#include "PluginProcessor.h"
#include "PluginEditor.h"

/**
 * This class is responsible for processing all the incoming audio (and MIDI data) for the plugin
 * and in this case, passing it on to Unity via UDP.
 */
JucetestoAudioProcessor::JucetestoAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", AudioChannelSet::stereo(), true)
                     #endif
                       ),
// A placeholder for a visual representation of the spectrogram (not currently in use)
spectrogramImage(Image::RGB, 512, 256, true),
// The GIST (https://github.com/adamstark/Gist) object for audio analysis.
// Currently used only for performing the FFT
gist(fftSize, gistSampleRate = 44100),
// Initialise the timer for MIDI messages
startTime(Time::getMillisecondCounterHiRes() * 0.001)
#endif
{
	// Declaration and initialisation for the plugin parameters
	// These are available for control via the DAW.
	// Controllable parameters for the left glove (flex sensors)
	addParameter(thumbParam = new AudioParameterInt(
		"thumb",
		"Thumb",
		0,
		127,
		63));
	addParameter(indexParam = new AudioParameterInt(
		"index",
		"Index",
		0,
		127,
		63));
	addParameter(ringParam = new AudioParameterInt(
		"ring",
		"Ring",
		0,
		127,
		63));

	// Controllable parameters for the right glove (piezos)
	addParameter(thumbParamDrums = new AudioParameterInt(
		"thumbDrums",
		"ThumbDrums",
		0,
		127,
		0));
	addParameter(indexParamDrums = new AudioParameterInt(
		"indexDrums",
		"IndexDrums",
		0,
		127,
		0));
	addParameter(middleParamDrums = new AudioParameterInt(
		"middleDrums",
		"middleDrums",
		0,
		127,
		0));
	addParameter(ringParamDrums = new AudioParameterInt(
		"ringDrums",
		"RingDrums",
		0,
		127,
		0));
	addParameter(pinkyParamDrums = new AudioParameterInt(
		"pinkyDrums",
		"PinkyDrums",
		0,
		127,
		0));

	// The socket the UDP receiver LISTENS to
	socket.bindToPort(1234);
	// The socket the UDP receiver WRITES to (not in use TODO check and delete)
	socket.write("127.0.0.1", 1235, "init", 4);
}

void JucetestoAudioProcessor::processBlock(AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
	
	//ScopedNoDenormals noDenormals;
	// Get the number of input and output channels
	// 2 for each in this case
	const auto totalNumInputChannels = getTotalNumInputChannels();
	const auto totalNumOutputChannels = getTotalNumOutputChannels();

	// In case we have more outputs than inputs, this code clears any output
	// channels that didn't contain input data, (because these aren't
	// guaranteed to be empty - they may contain garbage).
	// This is here to avoid people getting screaming feedback
	// when they first compile a plugin, but obviously you don't need to keep
	// this code if your algorithm always overwrites all the output channels.
	for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
		buffer.clear(i, 0, buffer.getNumSamples());

	// This is the place where you'd normally do the guts of your plugin's
	// audio processing...
	// Make sure to reset the state if your inner loop is processing
	// the samples and the outer loop is handling the channels.
	// Alternatively, you can process the samples with the channels
	// interleaved by keeping the same state.
 	auto* channelLeft = buffer.getWritePointer(0); // left
	auto* channelRight = buffer.getWritePointer(1); // right

	// Iterate over sample buffer and send samples to FFT FIFO buffer
	for (int sample = 0; sample < buffer.getNumSamples(); sample++)
	{
		// Send mono signal to FFT FIFO buffer
		pushNextSampleIntoFifo((channelLeft[sample] + channelRight[sample]) / 2);
	}
	// Send FFT data if buffer is full
	if (dataReady)
	{
		// SEND
		calculateAndSendData();
		// Reset guard
		dataReady = false;
	}

	// ---------------- MIDI ----------------
	// The current time for correct midi timecoding
	const auto currentTime = Time::getMillisecondCounterHiRes() * 0.001 - startTime;
	// Pinky finger triggers note on
	if (pinkyParamDrums->get() > 15 && !pinkyOn)
	{
		pinkyOn = true;
		MidiMessage m = MidiMessage::noteOn(channel, pinkyNoteNumber, 127.0f);
		processedMidi.addEvent(m, currentTime);
	}
	// Middle finger triggers note off (if currently playing)
	if (middleParamDrums->get() > 15 && pinkyOn)
	{
		pinkyOn = false;
		MidiMessage m = MidiMessage::noteOff(channel, pinkyNoteNumber);
		processedMidi.addEvent(m, currentTime);
	}
	
	// Send MIDI data
	midiMessages.swapWith(processedMidi);
}

// Destructor. Responsible for shutting down the UDP socket and clearing the FFT magnitude vector.
JucetestoAudioProcessor::~JucetestoAudioProcessor()
{
	socket.shutdown();
	gistMagnitudeVector.clear();
}

// Puts samples into fft FIFO until buffer is full (fftSize)
// Once the buffer is filled the fft is performed and the resulting data is sent to Unity
void JucetestoAudioProcessor::pushNextSampleIntoFifo(const float sample)
{
	if (gistIndex == fftSize)
	{
		// Reset index
		gistIndex = 0;
		gist.processAudioFrame(gistAudioFrame, fftSize);
		gistMagnitudeVector = gist.getMagnitudeSpectrum();
		pitch = gist.pitch(); // Not currently in use
		spectralCentroid = gist.spectralCentroid(); // Not currently in use
		dataReady = true; // Set guard, this triggers the FFT
	}

	// Put next sample into FIFO
	gistAudioFrame[gistIndex++] = sample;
}

// This method extracts audio information and sends it to Unity
// It gets triggered once the FIFO buffer is full
void JucetestoAudioProcessor::calculateAndSendData()
{
	// Calculate 7 average energy levels for that iteration
	// The FFT size is 2048, however to reduce the data sent over UDP the FFT
	// data is averaged into 7 broad frequency bands.

	// The index for array passed to the UDP data packet
	int idx = 0;

	// Extract FFT data
	const int step = fftSize / 2 / 7;
	for (auto y = 0; y < (fftSize / 2 - step); y += step)
	{
		float level = 0.0f;
		// Sum up values for that (broad) frequency band
		for (int i = y; i < y + step; i++)
		{
			level += gistMagnitudeVector[i];
		}
		udpData[idx] = int(level);
		idx++;
	}

	// Spectral centroid (not currently in use)
	udpData[idx++] = int(spectralCentroid);
	// Flex glove params
	udpData[idx++] = indexParam->get();
	udpData[idx++] = thumbParam->get();
	udpData[idx++] = ringParam->get();
	// Drum glove params
	udpData[idx++] = thumbParamDrums->get();
	udpData[idx++] = indexParamDrums->get();
	udpData[idx++] = middleParamDrums->get();
	udpData[idx++] = ringParamDrums->get();
	udpData[idx++] = pinkyParamDrums->get();

	// Send data to Unity via UDP
	socket.write("127.0.0.1", 1236, &udpData, idx);
}

int JucetestoAudioProcessor::map(int x, int from_min, int from_max, int to_min, int to_max)
{
	return (x - from_min) * (to_max - to_min) / (from_max - from_min) + to_min;
}

Image *JucetestoAudioProcessor::getSpectrogram()
{
	return &spectrogramImage;
}

const String JucetestoAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool JucetestoAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool JucetestoAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool JucetestoAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double JucetestoAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int JucetestoAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int JucetestoAudioProcessor::getCurrentProgram()
{
    return 0;
}

void JucetestoAudioProcessor::setCurrentProgram (int index)
{
}

const String JucetestoAudioProcessor::getProgramName (int index)
{
    return {};
}

void JucetestoAudioProcessor::changeProgramName (int index, const String& newName)
{
}

void JucetestoAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
}

void JucetestoAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool JucetestoAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
   #endif

    return true;
  #endif
}
#endif

bool JucetestoAudioProcessor::hasEditor() const
{
	return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* JucetestoAudioProcessor::createEditor()
{
	return new JucetestoAudioProcessorEditor(*this);
}

void JucetestoAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void JucetestoAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new JucetestoAudioProcessor();
}
