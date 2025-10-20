#include"PlayerGUI.h"
PlayerGUI::PlayerGUI()
{
	//Add buttons and slider
	for (auto* btn : { &playButton, &stopButton, &loadButton ,&restartButton })
	{
		btn->addListener(this);
		addAndMakeVisible(btn);
	}
	volumeSlider.setRange(0.0, 1.0, 0.01);
	volumeSlider.setValue(0.5);
	volumeSlider.addListener(this);
	addAndMakeVisible(volumeSlider);
}
PlayerGUI::~PlayerGUI() {};
void PlayerGUI::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
	 playerAudio.prepareToPlay(samplesPerBlockExpected, sampleRate);
}
 void PlayerGUI::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
	playerAudio.getNextAudioBlock(bufferToFill);
}
 void PlayerGUI::releaseResources()
{
	playerAudio.releaseResources();
 }
 void PlayerGUI::resized() //to show buttoms
 {
	 int y = 20;
	 loadButton.setBounds(20, y, 100, 40);//(x=20,y=20,width=100,height=40)
	 restartButton.setBounds(140, y, 80, 40);
	 stopButton.setBounds(240, y, 80, 40); 
	 volumeSlider.setBounds(20, 100, getWidth() - 40, 30);
	 playButton.setBounds(340,y,80,40);
 }
 void PlayerGUI::buttonClicked(juce::Button* button)//general fun for all buttons 
 {
	 if (button == &loadButton)
	 {
		 fileChooser = std::make_unique<juce::FileChooser>("Select an audio file to play...",
			 juce::File{},
			 "*.wav;*.mp3;*.aiff");
		 auto chooserFlags = juce::FileBrowserComponent::openMode
			 | juce::FileBrowserComponent::canSelectFiles;
		 fileChooser->launchAsync(chooserFlags, [this](const juce::FileChooser& fc)
			 {
				 auto file = fc.getResult();
				 if (file != juce::File{})
				 {
					 if (playerAudio.loadFile(file))
					 {
						 DBG("Loaded file: " + file.getFullPathName());
					 }
					 else
					 {
						 DBG("Failed to load file: " + file.getFullPathName());
					 }
				 }
			 });
	 }
	 else if (button == &playButton)
	 {
		 playerAudio.play();
	 }
	 else if (button == &stopButton)
	 {
		 playerAudio.stop();
	 }
 }
 void PlayerGUI::sliderValueChanged(juce::Slider* slider)
 {
	 if (slider == &volumeSlider)
	 {
		 playerAudio.setGain((float)volumeSlider.getValue());
	 }
 }
  

