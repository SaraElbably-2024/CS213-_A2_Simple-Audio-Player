#include "MainComponent.h"
  

MainComponent::MainComponent()
{
     
    addAndMakeVisible(playerGUI);
	addAndMakeVisible(playerGUI2);
    crossfader.setRange(0.0, 1.0, 0.01); 
    crossfader.setValue(crossfadeValue);
    crossfader.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    crossfader.addListener(this);
    addAndMakeVisible(crossfader);

    crossfaderLabel.setText("Mixer (Player 1 <---> Player 2)", juce::dontSendNotification);
    addAndMakeVisible(crossfaderLabel);

    setSize(800, 650); 
    setAudioChannels(0, 2);
    setSize(800, 650);

    setAudioChannels(0, 2);
}

MainComponent::~MainComponent()
{
    
    shutdownAudio();
}

 
void MainComponent::sliderValueChanged(juce::Slider* slider)
{
    if (slider == &crossfader)
    {
        crossfadeValue = (float)crossfader.getValue();

    
        float gain1 = 1.0f - crossfadeValue;

    
        float gain2 = crossfadeValue;
        playerGUI.setGain(gain1);
        playerGUI2.setGain(gain2);
    }
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
    int segmentHeight = area.getHeight() / 3;

    
    auto playerArea1 = area.removeFromTop(segmentHeight + 50); 
    playerGUI.setBounds(playerArea1);

   
    auto playerArea2 = area.removeFromTop(segmentHeight + 50); 
    playerGUI2.setBounds(playerArea2);

   
    auto mixerArea = area.reduced(20);

    crossfaderLabel.setBounds(mixerArea.removeFromTop(20));
    crossfader.setBounds(mixerArea.removeFromTop(50));
   
}
 
