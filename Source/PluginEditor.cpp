#include "PluginEditor.h"
#include "PluginProcessor.h"

JucetestoAudioProcessorEditor::JucetestoAudioProcessorEditor (JucetestoAudioProcessor& p)
    : AudioProcessorEditor(&p), processor(p)
// , spec(p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (800, 600);

	/*
	// auto& params = processor.getParameters();	
	//AudioParameterFloat* gainParameter = static_cast<AudioParameterFloat*>(params.getUnchecked(0));
	
	/*mGainControlSlider.setBounds(0, 0, 100, 100);
	mGainControlSlider.setSliderStyle(Slider::SliderStyle::RotaryVerticalDrag);
	mGainControlSlider.setTextBoxStyle(Slider::NoTextBox, true, 0, 0);
	mGainControlSlider.setRange(gainParameter->range.start, gainParameter->range.end);
	mGainControlSlider.setValue(*gainParameter);
	mGainControlSlider.addListener(this);
	addAndMakeVisible(mGainControlSlider);*/

	// Spectrogram
	//spec.setBounds(110, 24, 512, 512);
	//addAndMakeVisible(spec);
}

void JucetestoAudioProcessorEditor::paint (Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));

    g.setColour (Colours::white);
    g.setFont (15.0f);
    g.drawFittedText ("Helloooos", getLocalBounds(), Justification::centred, 1);
}

void JucetestoAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
}

//void JucetestoAudioProcessorEditor::sliderValueChanged(Slider* slider)
//{
//	auto& params = processor.getParameters();
//	
//	// As more sliders are added, this if clause grows
//	if(slider == &mGainControlSlider)
//	{
//		AudioParameterFloat* gainParameter = static_cast<AudioParameterFloat*>(params.getUnchecked(0));
//		*gainParameter = mGainControlSlider.getValue();
//		DBG(slider->getValue());
//	}
//}
