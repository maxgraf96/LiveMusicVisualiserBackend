#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "gist/src/Gist.h"

/**
 * This class is responsible for processing all the incoming audio (and MIDI data) for the plugin
 * and in this case, passing it on to Unity via UDP.
 */
class JucetestoAudioProcessor  : public AudioProcessor
{
public:
	// ------------- BEGIN JUCE related fields and methods -------------
    JucetestoAudioProcessor();
    ~JucetestoAudioProcessor();

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (AudioBuffer<float>&, MidiBuffer&) override;

    AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    const String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const String getProgramName (int index) override;
    void changeProgramName (int index, const String& newName) override;

    void getStateInformation (MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

	// ------------- END JUCE related fields and methods -------------

	// Spectrogram related stuff (not currently in use)
	Image *getSpectrogram();

	// Size of the FFT
	enum
	{
		fftOrder = 11,
		fftSize = 1 << fftOrder
	};

	// MIDI
	// Start time of the plugin saved to correctly timestamp MIDI messages
	double startTime;

private:
	// Flex glove parameters
	AudioParameterInt* indexParam;
	AudioParameterInt* thumbParam;
	AudioParameterInt* ringParam;

	// Drum glove parameters
	AudioParameterInt* thumbParamDrums;
	AudioParameterInt* indexParamDrums;
	AudioParameterInt* middleParamDrums;
	AudioParameterInt* ringParamDrums;
	AudioParameterInt* pinkyParamDrums;
	
	// The JUCE UDP socket for communicating with Unity
	DatagramSocket socket;
	// Array storing the data passed to Unity
	unsigned char udpData[16];
	
	// Spectrogram image (not currently in use)
	Image spectrogramImage;

	// Puts samples into fft FIFO until buffer is full (fftSize)
	// Once the buffer is filled the fft is performed and the resulting data is sent to Unity
	void pushNextSampleIntoFifo(float sample);

	// This method extracts audio information and sends it to Unity
	// It gets triggered once the FIFO buffer is full
	void calculateAndSendData();

	// GIST object for audio analysis (https://github.com/adamstark/Gist)
	Gist<float> gist;
	// The frame size for the GIST object. In this case it is equal to the FFT size,
	// as currently the only operation performed by the GIST object is the FFT.
	float gistAudioFrame[fftSize];

	// Set the sample rate for GIST
	int gistSampleRate = 44100;
	int gistIndex = 0;
	// Guard for sending data to Unity
	bool dataReady = false;
	// Interim storage for fft magnitude processing
	std::vector<float> gistMagnitudeVector;
	// Pitch and spectral centroid variables (not currently in use)
	float pitch, spectralCentroid;

	// MIDI
	// The MIDI buffer. Takes MIDI data and sends it to the DAW.
	MidiBuffer processedMidi;
	// Flag for if the crash (pinky finger) was triggered and is currently playing
	bool pinkyOn = false;
	// The MIDI channel of operation
	const int channel = 1;
	// The MIDI note number sent to the DAW.
	int pinkyNoteNumber = 69; // 440Hz

	// Helper methods
	// Maps an given integer from a given range to a given range
	static int map(int x, int from_min, int from_max, int to_min, int to_max);

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(JucetestoAudioProcessor);
};