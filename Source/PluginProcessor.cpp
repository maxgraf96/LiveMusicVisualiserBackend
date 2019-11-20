#include "PluginProcessor.h"
#include "PluginEditor.h"

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
spectrogramImage(Image::RGB, 512, 256, true),
gist(fftSize, gistSampleRate = 44100)
#endif
{
	addParameter(indexParam = new AudioParameterInt(
		"index",
		"Index",
		0,
		127,
		63));
	
	socket.bindToPort(1234);
	socket.write("127.0.0.1", 1235, "init", 4);
}

void JucetestoAudioProcessor::processBlock(AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
	ScopedNoDenormals noDenormals;
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

	for (int sample = 0; sample < buffer.getNumSamples(); sample++)
	{
		pushNextSampleIntoFifo(channelLeft[sample]);
	}
	// Send FFT data if buffer is full
	if (dataReady)
	{
		calculateAndSendData();
		dataReady = false;
	}

	//// ---------------- MIDI ----------------
	//MidiBuffer processedMidi;
	//int time;
	//MidiMessage m;
	//for (MidiBuffer::Iterator i(midiMessages); i.getNextEvent(m, time);)
	//{
	//	if (m.isNoteOn())
	//	{
	//		uint8 newVel = (uint8) indexValue;
	//		m = MidiMessage::noteOn(m.getChannel(), m.getNoteNumber(), newVel);
	//	}
	//	else if (m.isNoteOff())
	//	{
	//	}
	//	else if (m.isAftertouch())
	//	{
	//	}
	//	else if (m.isPitchWheel())
	//	{
	//	}
	//	processedMidi.addEvent(m, time);
	//}
	//midiMessages.swapWith(processedMidi);
}

JucetestoAudioProcessor::~JucetestoAudioProcessor()
{
	socket.shutdown();
	gistMagnitudeVector.clear();
}

void JucetestoAudioProcessor::pushNextSampleIntoFifo(const float sample)
{
	if (gistIndex == fftSize)
	{
		// Reset index
		gistIndex = 0;
		gist.processAudioFrame(gistAudioFrame, fftSize);
		gistMagnitudeVector = gist.getMagnitudeSpectrum();
		pitch = gist.pitch();
		spectralCentroid = gist.spectralCentroid();
		dataReady = true;
	}

	gistAudioFrame[gistIndex++] = sample;
}

void JucetestoAudioProcessor::calculateAndSendData()
{
	// Calculate 7 average energy levels for that iteration (7 frequency components => 7 cubes in Unity)
	int idx = 0;

	// FFT data
	const int step = fftSize / 2 / 7;
	for (auto y = 0; y < (fftSize / 2 - step); y += step)
	{
		float level = 0.0f;
		for (int i = y; i < y + step; i++)
		{
			level += gistMagnitudeVector[i];
		}
		udpData[idx] = int(level);
		idx++;
	}

	// Spectral centroid
	udpData[idx++] = int(spectralCentroid);
	udpData[idx++] = indexParam->get();

	// Send data to Unity via UDP
	socket.write("127.0.0.1", 1236, &udpData, idx);
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
