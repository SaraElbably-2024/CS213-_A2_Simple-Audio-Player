#include "MainComponent.h"
  

MainComponent::MainComponent()
{
     
    addAndMakeVisible(playerGUI);
	addAndMakeVisible(playerGUI2);

      
    setSize(800, 350);

    setAudioChannels(0, 2);//set 0 input channel & open 2 output channel(stereo)
}

MainComponent::~MainComponent()
{
    
    shutdownAudio();
}

 

void MainComponent::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{

    
    playerGUI.prepareToPlay(samplesPerBlockExpected, sampleRate);
	playerGUI2.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void MainComponent::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
     
    bufferToFill.clearActiveBufferRegion();
    playerGUI.getNextAudioBlock(bufferToFill);
	juce::AudioBuffer<float> tempBuffer(bufferToFill.buffer->getNumChannels(), bufferToFill.numSamples);
    juce::AudioSourceChannelInfo info2;
	info2.buffer = &tempBuffer;
	info2.startSample = 0;
	info2.numSamples = bufferToFill.numSamples;
    playerGUI2.getNextAudioBlock(info2);
    for (int channel = 0; channel < bufferToFill.buffer->getNumChannels(); ++channel)
    {
        bufferToFill.buffer->addFrom(channel, bufferToFill.startSample,
            tempBuffer,
            channel, 0,
			bufferToFill.numSamples);
	}
}

void MainComponent::releaseResources()
{
   
    playerGUI.releaseResources();
	playerGUI2.releaseResources();
}

 

void MainComponent::paint(juce::Graphics& g)
{
     
    g.fillAll(juce::Colours::lightpink);
    g.setFont(40.0f);
    g.setColour(juce::Colours::deeppink);
    g.drawText("AUDIO PLAYER", getLocalBounds(), juce::Justification::centred);
}

void MainComponent::resized()
{
    auto area = getLocalBounds();
	playerGUI.setBounds(area.removeFromTop(area.getHeight() / 2));
    
    playerGUI2.setBounds(area);

   
}
 
