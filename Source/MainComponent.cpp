#include "MainComponent.h"
  

MainComponent::MainComponent()
{
     
    addAndMakeVisible(playerGUI);

      
    setSize(500, 250);

    setAudioChannels(0, 2);
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
}

void MainComponent::resized()
{
    
    playerGUI.setBounds(getLocalBounds());

   
}
 
