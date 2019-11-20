#pragma once
#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"

class SpectrogramComponent : public Component
//, Timer
{
public:
	SpectrogramComponent(JucetestoAudioProcessor&);

	~SpectrogramComponent() override = default;
	
	void paint(Graphics& g) override;

private:
	Image spectrogramImage;
	JucetestoAudioProcessor& processor;
};
