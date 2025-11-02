#include "PlayerGUI.h"
#include<JuceHeader.h>
using namespace std;

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
                       &pauseButton ,&endButton,  &LoopButton , &setBButton , &clearABButton,&setAButton,&nextButton,&previousButton })
    {
        btn->addListener(this);
        addAndMakeVisible(btn);
    }
    //info label
    metadataLabel.setText("No file loaded", juce::dontSendNotification);

    metadataLabel.setJustificationType(juce::Justification::centred);
    metadataLabel.setColour(juce::Label::textColourId, juce::Colours::black);
    metadataLabel.setColour(juce::Label::backgroundColourId, juce::Colours::lightgrey);
    addAndMakeVisible(metadataLabel);

    //playlist box
    playlistBox.setModel(this);
    addAndMakeVisible(playlistBox);

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
    //progress bar
    
    progressBar.setSliderStyle(juce::Slider::LinearHorizontal);
    progressBar.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);  
    progressBar.setEnabled(false);  
    addAndMakeVisible(progressBar);
    addAndMakeVisible(progressTimeLabal);
    // ===== ألوان ال progress bar =====
    progressBar.setColour(juce::Slider::trackColourId, juce::Colours::darkgrey);
    progressBar.setColour(juce::Slider::thumbColourId, juce::Colours::deeppink); 

    //----------
}

PlayerGUI::~PlayerGUI() {};

void PlayerGUI::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    playerAudio.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void PlayerGUI::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    playerAudio.getNextAudioBlock(bufferToFill);
    //auto advance when track ends
    if (currentIndex >= 0 && !isLooping)
    {
        double pos = playerAudio.getPosition();
        double len = playerAudio.getLength();

       
        if (len > 0.0 && pos >= (len - 0.03))
        {
            // ensure we call UI changes & file loading on message thread
            juce::MessageManager::callAsync([this]() {
                playNextInPlaylist();
                });
        }
    }




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
    int buttonWidth = buttonArea.getWidth() / 14; // 14 زر
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
    clearABButton.setBounds(x, 0, buttonWidth, rowHeight); x += buttonWidth;
    nextButton.setBounds(x, 0, buttonWidth, rowHeight); x += buttonWidth;
    previousButton.setBounds(x, 0, buttonWidth, rowHeight); 
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
    // progress bar  
    auto progressArea = area.removeFromTop(30).reduced(margin);

     
    int timeWidth = 100;  
    progressTimeLabal.setBounds(progressArea.getX(), progressArea.getY(), timeWidth, progressArea.getHeight());
    progressTimeLabal.setColour(juce::Label::textColourId, juce::Colours::white);
    progressTimeLabal.setJustificationType(juce::Justification::centredRight);

     
    progressBar.setBounds(progressArea.getX() + timeWidth + 5, progressArea.getY(),
        progressArea.getWidth() - timeWidth - 5, progressArea.getHeight());

    // metadata label 
    metadataLabel.setBounds(area.removeFromTop(24).reduced(margin));

    // playlist occupies the remaining bottom area
    playlistBox.setBounds(area.reduced(margin));
}

void PlayerGUI::buttonClicked(juce::Button* button)
{
    if (button == &loadButton)
    {
        fileChooser = std::make_unique<juce::FileChooser>("Select an audio file to play...",
            juce::File{},
            "*.wav;*.mp3;*.aiff;*.flac;*.ogg;*.m4a"); 
        

        auto chooserFlags = juce::FileBrowserComponent::openMode
            | juce::FileBrowserComponent::canSelectFiles
            | juce::FileBrowserComponent::canSelectMultipleItems;

        fileChooser->launchAsync(chooserFlags, [this](const juce::FileChooser& fc)
            {
                auto files = fc.getResults();
                if (files.size() > 0)
                {
                    bool wasEmpty = playlistFiles.size() == 0;
                    for (auto& f : files)
                    {
                        if (f.existsAsFile())
                        {
                            playlistFiles.add(f);
                        }
                    }

                    playlistBox.updateContent();

                    // If playlist was empty before and we added files, auto-play first one
                    if (wasEmpty && playlistFiles.size() > 0)
                    {
                        playFileAtIndex(0);
                    }
                }
            });
    }
    else if (button == &playButton)
    {
        if (currentIndex >= 0 && currentIndex < playlistFiles.size())
        {
            playerAudio.play();
            updateMetadataLabel();
        }
        else
        {
            // if nothing selected but playlist non-empty, play first
            if (playlistFiles.size() > 0)
            {
                playFileAtIndex(0);
            }
        }
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
    else if (button == &nextButton)
    {
        playNextInPlaylist();
        }
    else if (button == &previousButton)
    {
        playPreviousInPlaylist();
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

    if (totalLength > 0.0)
    {
        // position slider
        positionSlider.setRange(0.0, totalLength, 0.001);
        if (!positionSlider.isMouseButtonDown()) {
            positionSlider.setValue(currentPosition, juce::dontSendNotification);
        }
        juce::String timeString = secondsToTimeString(currentPosition) + " / " + secondsToTimeString(totalLength);
        timeLabel.setText(timeString, juce::dontSendNotification);
        //progress Bar
        progressBar.setRange(0.0, totalLength, 0.001);
        progressBar.setValue(currentPosition, juce::dontSendNotification);
         
        juce::String progressText = secondsToTimeString(totalLength - currentPosition) + " / " +
            secondsToTimeString(currentPosition);
        progressTimeLabal.setText(progressText, juce::dontSendNotification);
        //-----------
    }
    else
    {
        positionSlider.setRange(0.0, 1.0, 0.001);
        positionSlider.setValue(0.0, juce::dontSendNotification);
        timeLabel.setText("00:00 / 00:00", juce::dontSendNotification);

        //progress bar  
        progressBar.setRange(0.0, 1.0, 0.001);
        progressBar.setValue(0.0, juce::dontSendNotification);
        progressTimeLabal.setText("00:00 / 00:00", juce::dontSendNotification);
    }
}



// ListBoxModel implementations for playlist display
int PlayerGUI::getNumRows()
{
    return (int)playlistFiles.size();
}

void PlayerGUI::paintListBoxItem(int rowNumber, juce::Graphics& g, int width, int height, bool rowIsSelected)
{
    if (rowNumber < 0 || rowNumber >= playlistFiles.size())
        return;

    auto file = playlistFiles.getReference(rowNumber);
    juce::String name = file.getFileName();

    if (rowIsSelected)
    {
        g.fillAll(juce::Colours::lightblue);
    }

    g.setColour(juce::Colours::black);
    g.setFont(height * 0.6f);
    g.drawText(name, 4, 0, width - 4, height, juce::Justification::centredLeft, true);
}

void PlayerGUI::listBoxItemClicked(int row, const juce::MouseEvent& )
{
    if (row >= 0 && row < playlistFiles.size())
    {
        playFileAtIndex(row);
    }
}

void PlayerGUI::updateMetadataLabel()
{
    // update info label text from playerAudio
    juce::String info = playerAudio.getFileInfo();
    if (info.isEmpty() && currentIndex >= 0 && currentIndex < playlistFiles.size())
        info = playlistFiles[currentIndex].getFileName();

    metadataLabel.setText(info, juce::dontSendNotification);
    playlistBox.selectRow(currentIndex);
}

void PlayerGUI::playFileAtIndex(int index)
{
    if (index < 0 || index >= playlistFiles.size())
        return;

    currentIndex = index;
    auto file = playlistFiles.getReference(index);

    // Load the file on message thread (we are on message thread here)
    if (playerAudio.loadFile(file))
    {
        updateMetadataLabel();
        // ensure selection visible
        playlistBox.selectRow(currentIndex);
    }
    else
    {
        metadataLabel.setText("Failed to load: " + file.getFileName(), juce::dontSendNotification);
    }
}

void PlayerGUI::playNextInPlaylist()
{
    if (playlistFiles.size() == 0)
        return;

    int next = currentIndex + 1;
    if (next >= playlistFiles.size())
        next = 0; // wrap-around, or choose to stop instead
    playFileAtIndex(next);
}

void PlayerGUI::playPreviousInPlaylist()
{
    if (playlistFiles.size() == 0)
        return;

    int prev = currentIndex - 1;
    if (prev < 0)
        prev = (int)playlistFiles.size() - 1;
    playFileAtIndex(prev);
}


















void PlayerGUI::paint(juce::Graphics& g)
{
    g.fillAll(juce::Colour(30, 30, 30));
    g.setColour(juce::Colours::deeppink);
    g.drawRect(getLocalBounds(), 2);
}
