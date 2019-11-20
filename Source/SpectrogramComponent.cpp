#include "SpectrogramComponent.h"

SpectrogramComponent::SpectrogramComponent(JucetestoAudioProcessor& p) :
	spectrogramImage(Image::RGB, 512, 512, true),
	processor(p)
{
	setOpaque(true);
	setSize(700, 500);
}

void SpectrogramComponent::paint(Graphics& g)
{
	g.fillAll(Colours::black);

	g.setOpacity(1.0f);
	//g.drawImage(spectrogramImage, getLocalBounds().toFloat());
}