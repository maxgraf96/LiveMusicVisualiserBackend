#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"
#include "SpectrogramComponent.h"

class JucetestoAudioProcessorEditor : public AudioProcessorEditor
// , public Slider::Listener
{
public:
    JucetestoAudioProcessorEditor (JucetestoAudioProcessor&);
    ~JucetestoAudioProcessorEditor() = default;

    void paint (Graphics&) override;
    void resized() override;
	//void sliderValueChanged(Slider* slider) override;

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    JucetestoAudioProcessor& processor;

	//SpectrogramComponent spec;
	//Slider mGainControlSlider;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (JucetestoAudioProcessorEditor)
};
