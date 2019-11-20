#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "gist/src/Gist.h"

class JucetestoAudioProcessor  : public AudioProcessor
{
public:
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

	// Spectrogram related stuff
	Image *getSpectrogram();

	enum
	{
		fftOrder = 11,
		fftSize = 1 << fftOrder
	};

	// MIDI
	int indexValue;

private:
	AudioParameterInt* indexParam;

	int rData[fftSize / 2];
	int gData[fftSize / 2];
	int bData[fftSize / 2];
	
	DatagramSocket socket;
	unsigned char udpData[7];
	
	// Spectrogram related stuff
	Image spectrogramImage;

	void pushNextSampleIntoFifo(float sample);
	void calculateAndSendData();

	// Gist
	Gist<float> gist;
	float gistAudioFrame[fftSize];
	
	int gistSampleRate = 44100;
	int gistIndex = 0;
	bool dataReady = false;
	std::vector<float> gistMagnitudeVector;
	float pitch, spectralCentroid;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(JucetestoAudioProcessor);
};