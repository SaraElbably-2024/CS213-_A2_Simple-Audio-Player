#include "PlayerGUI.h"

juce::String PlayerGUI::secondsToTimeString(double seconds) {
    if (seconds < 0) { seconds = 0; }
    int totalSeconds = juce::roundToInt(seconds);
    int minutes = totalSeconds / 60;
    int remainingSeconds = totalSeconds % 60;
    return juce::String(minutes) + ":" + juce::String(remainingSeconds).paddedLeft('0', 2);
}

PlayerGUI::PlayerGUI()
{
    setSize(1250, 220);

    //Add buttons and slider
    for (auto* btn : { &playButton, &stopButton, &loadButton ,&muteButton , &restartButton ,&gotostartButton,
                       &pauseButton ,&endButton,  &LoopButton , &setBButton , &clearABButton,&setAButton })
    {
        btn->addListener(this);
        addAndMakeVisible(btn);
    }

    positionSlider.setRange(0.0, 1.0, 0.001);
    positionSlider.addListener(this);
    addAndMakeVisible(positionSlider);

    timeLabel.setText("00:00 / 00:00", juce::dontSendNotification);
    timeLabel.setColour(juce::Label::textColourId, juce::Colours::lightpink);
    timeLabel.setColour(juce::Label::backgroundColourId, juce::Colours::whitesmoke);
    addAndMakeVisible(timeLabel);

    startTimerHz(30);

    //speed====
    speedSlider.setRange(0.5, 2.0, 0.01);
    speedSlider.setValue(1.0);
    speedSlider.addListener(this);
    addAndMakeVisible(speedSlider);

    //speed====
    speedLabel.setText("Speed", juce::dontSendNotification);
    addAndMakeVisible(speedLabel);
    //------
    //sleeptime
    addAndMakeVisible(sleepTimeEditor);
    sleepTimeEditor.setText("10");  
    sleepTimeEditor.setInputRestrictions(5, "0123456789");  

    addAndMakeVisible(startSleepButton);
    startSleepButton.setButtonText("Set Sleep Timer");
    startSleepButton.onClick = [this]() {
        auto text = sleepTimeEditor.getText();
        double seconds = text.getDoubleValue();

        if (seconds > 0)
            playerAudio.startSleepTimer(seconds);
        };
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
void PlayerGUI::setGain(float gain)
{
    playerAudio.setGain(gain);
  
    }
void PlayerGUI::resized()
{
    
    auto area = getLocalBounds();
    int y = 5; 
    int rowHeight = 35;
    int margin = 5;

  
    auto buttonArea = area.removeFromTop(rowHeight + 5).reduced(margin);
    int buttonWidth = buttonArea.getWidth() / 12; // 12 زر
    int x = 0;

   
    loadButton.setBounds(x, 0, buttonWidth, rowHeight); x += buttonWidth;
    playButton.setBounds(x, 0, buttonWidth, rowHeight); x += buttonWidth;
    pauseButton.setBounds(x, 0, buttonWidth, rowHeight); x += buttonWidth;
    muteButton.setBounds(x, 0, buttonWidth, rowHeight); x += buttonWidth;
    restartButton.setBounds(x, 0, buttonWidth, rowHeight); x += buttonWidth;
    gotostartButton.setBounds(x, 0, buttonWidth, rowHeight); x += buttonWidth;
    endButton.setBounds(x, 0, buttonWidth, rowHeight); x += buttonWidth;
    stopButton.setBounds(x, 0, buttonWidth, rowHeight); x += buttonWidth;
    LoopButton.setBounds(x, 0, buttonWidth, rowHeight); x += buttonWidth;
    setAButton.setBounds(x, 0, buttonWidth, rowHeight); x += buttonWidth;
    setBButton.setBounds(x, 0, buttonWidth, rowHeight); x += buttonWidth;
    clearABButton.setBounds(x, 0, buttonWidth, rowHeight);

    // ---------------------------------------------------------------------

    positionSlider.setBounds(area.removeFromTop(30).reduced(margin));

   
    timeLabel.setBounds(area.removeFromTop(20).reduced(margin));

    // ---------------------------------------------------------------------

    
    auto speedArea = area.removeFromTop(30).reduced(margin);
    speedLabel.setBounds(speedArea.removeFromLeft(80));
    speedSlider.setBounds(speedArea);

    // ---------------------------------------------------------------------

    
    auto sleepArea = area.removeFromTop(40).reduced(margin); 

    
    sleepTimeEditor.setBounds(sleepArea.removeFromLeft(100));

  
    startSleepButton.setBounds(sleepArea.removeFromLeft(150));

    
}

void PlayerGUI::buttonClicked(juce::Button* button)
{
    if (button == &loadButton)
    {
        fileChooser = std::make_unique<juce::FileChooser>("Select an audio file to play...",
            juce::File{},
            "*.wav;*.mp3;*.aiff;*.flac;*.ogg;*.m4a"); 
        

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
    else if (button == &LoopButton) {
        isLooping = !isLooping;
        playerAudio.setLooping(isLooping);
        if (isLooping) {
            LoopButton.setButtonText("Loop ON");
            DBG("Looping is ON");
        }
        else {
            LoopButton.setButtonText("Loop OFF");
            DBG("Looping is OFF");
        }
    }
    else if (button == &restartButton) {
        playerAudio.restart();
    }
    else if (button == &stopButton)
    {
        playerAudio.stop();
    }
    else if (button == &muteButton)
    {
        if (playerAudio.getLength() > 0)
        {
            if (!muted)
            {
             
                previousVolume = playerAudio.getGain(); 

                setGain(0.0f);
                muteButton.setButtonText("Unmute");
                muted = true;
            }
            else
            {
             
                if (previousVolume <= 0.0f)
                {
                    previousVolume = 0.5f;
                }

                setGain(previousVolume);
                muteButton.setButtonText("Mute");
                muted = false;
            }
        }
        else
        {
            DBG("No audio file loaded!");
            return;
        }
    }
    else if (button == &pauseButton)
    {
        playerAudio.pause();
    }
    else if (button == &gotostartButton)
    {
        playerAudio.restart();
    }
    else if (button == &endButton)
    {
        playerAudio.stop();
        playerAudio.setPosition(0.0);
    }
    else if (button == &setAButton) {
        loopStartA = playerAudio.getPosition();
        if (loopEndB != -1.0 && loopStartA >= loopEndB) {
            loopEndB = -1.0;
            setBButton.setButtonText("Set B");
            DBG("A set , B cleared");
        }
        setAButton.setButtonText(" A Set: " + secondsToTimeString(loopStartA));
        DBG("Loop start A set at: " + juce::String(loopStartA));
        isLooping = false;
        LoopButton.setButtonText("Loop OFF");
        playerAudio.setSegmentLoop(loopStartA, loopEndB);
    }
    else if (button == &setBButton) {
        double currentPos = playerAudio.getPosition();
        if (loopStartA != -1.0 && currentPos > loopStartA) {
            loopEndB = currentPos;
            setBButton.setButtonText(" B Set: " + secondsToTimeString(loopEndB));
            DBG("Loop end B set at: " + juce::String(loopEndB));
            isLooping = false;
            LoopButton.setButtonText("Loop OFF");
            playerAudio.setSegmentLoop(loopStartA, loopEndB);
        }
        else {
            DBG("Cannot set B : B must be after a set A point.");
        }
    }
    else if (button == &clearABButton) {
        loopStartA = -1.0;
        loopEndB = -1.0;
        playerAudio.setSegmentLoop(loopStartA, loopEndB);
        setAButton.setButtonText("Set A");
        setBButton.setButtonText("Set B");
        DBG("Loop points A and B cleared.");
    }
}

void PlayerGUI::sliderValueChanged(juce::Slider* slider)
{
   
    if (slider == &positionSlider)
    {
        double newPosition = positionSlider.getValue();
        playerAudio.setPosition(newPosition);

        double totalLength = playerAudio.getLength();
        juce::String timeString = secondsToTimeString(newPosition) + " / " + secondsToTimeString(totalLength);
        timeLabel.setText(timeString, juce::dontSendNotification);
    }
    //speed====
    else if (slider == &speedSlider)
    {
        playerAudio.setSpeed(speedSlider.getValue());
    }
}

void PlayerGUI::timerCallback()
{
    //sleeptime
    playerAudio.checkSleepTimer();
    //------
    double currentPosition = playerAudio.getPosition();
    double totalLength = playerAudio.getLength();
    if (totalLength > 0.0) {
        positionSlider.setRange(0.0, totalLength, 0.001);
        if (!positionSlider.isMouseButtonDown()) {
            positionSlider.setValue(currentPosition, juce::dontSendNotification);
        }

        juce::String timeString = secondsToTimeString(currentPosition) + " / " + secondsToTimeString(totalLength);
        timeLabel.setText(timeString, juce::dontSendNotification);
    }
    else {
        positionSlider.setRange(0.0, 1.0, 0.001);
        positionSlider.setValue(0.0, juce::dontSendNotification);
        timeLabel.setText("00:00 / 00:00", juce::dontSendNotification);
    }
}

void PlayerGUI::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(30, 30, 30));
    g.setColour(juce::Colours::deeppink);
    g.drawRect(getLocalBounds(), 2);
}
