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
	
    juce::Colour orangeColour = juce::Colour::fromString("#FFB25200");
    juce::Colour lightOrange = orangeColour.brighter(0.5f);

    crossfader.setColour(juce::Slider::thumbColourId, lightOrange);
    crossfader.setColour(juce::Slider::trackColourId, orangeColour);

    crossfaderLabel.setText("Mixer (Player 1 <---> Player 2)", juce::dontSendNotification);
    crossfaderLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    addAndMakeVisible(crossfaderLabel);

    crossfader.setRange(0.0, 1.0, 0.01);
    crossfader.setValue(crossfadeValue);
    crossfader.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    crossfader.addListener(this);
    addAndMakeVisible(crossfader);

	//crossfader colour
    crossfader.setColour(juce::Slider::thumbColourId, lightOrange); 
    crossfader.setColour(juce::Slider::trackColourId, orangeColour); 

    crossfaderLabel.setText("Mixer (Player 1 <---> Player 2)", juce::dontSendNotification);
    crossfaderLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    addAndMakeVisible(crossfaderLabel);
    // ...

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
    
    g.fillAll(juce::Colour(30, 30, 30));

 
    juce::Colour orangeColour = juce::Colour::fromString("#FFB25200");
    g.setColour(orangeColour);
    g.drawRect(getLocalBounds(), 10); 

   
    g.setFont(40.0f);

    
    g.setColour(juce::Colours::white);

  
    g.drawText("Simple Audio Player",
        getLocalBounds().removeFromTop(100),
        juce::Justification::centred);     
}

void MainComponent::resized()
{
    auto fullArea = getLocalBounds();
    fullArea.removeFromTop(100);
    int playerWidth = 950;
    int playerHeight = 300; 
    int totalHeight = playerHeight * 2 + 10 + 10 + 30;

    auto centerContainer = fullArea.withHeight(totalHeight).withWidth(playerWidth);

    
    centerContainer.setCentre(fullArea.getCentreX(), fullArea.getCentreY());

    
    auto currentArea = centerContainer;
    playerGUI.setBounds(currentArea.removeFromTop(playerHeight));
    currentArea.removeFromTop(10);
    playerGUI2.setBounds(currentArea.removeFromTop(playerHeight));
    currentArea.removeFromTop(10);
    crossfaderLabel.setBounds(currentArea.removeFromTop(15));
    crossfader.setBounds(currentArea.removeFromTop(15));
}