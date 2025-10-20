#include "MainComponent.h"
  

MainComponent::MainComponent()
{
     
    addAndMakeVisible(playerGUI);

      
    setSize(500, 250);

    setAudioChannels(0, 2);//set 0 input channel & open 2 output channel(stereo)
}

MainComponent::~MainComponent()
{
    
    shutdownAudio();
}

 

void MainComponent::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    
    playerGUI.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void MainComponent::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
     
    bufferToFill.clearActiveBufferRegion();
    playerGUI.getNextAudioBlock(bufferToFill);
}

void MainComponent::releaseResources()
{
   
    playerGUI.releaseResources();
}

 

void MainComponent::paint(juce::Graphics& g)
{
     
    g.fillAll(juce::Colours::darkgrey);
    g.setFont(40.0f);
    g.setColour(juce::Colours::black);
    g.drawText("AUDIO PLAYER", getLocalBounds(), juce::Justification::centred);
}

void MainComponent::resized()
{
    
    playerGUI.setBounds(getLocalBounds());

   
}
 
