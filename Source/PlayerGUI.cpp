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
	// volume slider
  
    volumeSlider.setRange(0.0, 1.0, 0.01);
    volumeSlider.setValue(0.5);
    volumeSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    volumeSlider.addListener(this);
    addAndMakeVisible(volumeSlider);

    // Volume Label 💡
    volumeLabel.setText("Volume", juce::dontSendNotification);
    addAndMakeVisible(volumeLabel);

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
    // colourssssssssssss
    //----------
    juce::Colour orangeColour = juce::Colour::fromString("#FFB25200"); // البرتقالي الداكن للأزرار
    juce::Colour lightOrange = orangeColour.brighter(0.5f); // البرتقالي الفاتح للمؤشرات

    // 1. تطبيق الألوان على الأزرار
    for (auto* btn : { &loadButton, &playButton, &pauseButton, &muteButton, &restartButton, &gotostartButton,
                       &endButton, &stopButton, &LoopButton, &setAButton, &setBButton, &clearABButton,
                       &nextButton, &previousButton, &startSleepButton })
    {
        btn->setColour(juce::TextButton::buttonColourId, orangeColour);
        btn->setColour(juce::TextButton::buttonOnColourId, orangeColour.darker(0.3f));
        btn->setColour(juce::TextButton::textColourOffId, juce::Colours::white);
        btn->setColour(juce::TextButton::textColourOnId, juce::Colours::white);
    }

    // 2. تطبيق الألوان على مؤشرات Sliders والعناوين
    positionSlider.setColour(juce::Slider::thumbColourId, lightOrange);
    positionSlider.setColour(juce::Slider::trackColourId, juce::Colours::darkgrey);

    speedSlider.setColour(juce::Slider::thumbColourId, lightOrange);
    volumeSlider.setColour(juce::Slider::thumbColourId, lightOrange);

    // شريط التقدم (ProgressBar)
    progressBar.setColour(juce::Slider::thumbColourId, lightOrange);

    // عناوين Labels باللون الأبيض
    speedLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    volumeLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    timeLabel.setColour(juce::Label::textColourId, juce::Colours::white);
    progressTimeLabal.setColour(juce::Label::textColourId, juce::Colours::white);


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
   // playerAudio.setGain(gain);
    currentCrossfadeGain = gain;
    float internalGain = volumeSlider.getValue();
    playerAudio.setInternalVolume(internalGain);
    playerAudio.setGain(currentCrossfadeGain * internalGain);
    }
// الملف: PlayerGUI.cpp

void PlayerGUI::resized()
{
    // 1. تحديد الهامش الداكن الموحد (20 بكسل)
    // هذا يضمن أن المسافة بين حدود المكون الداكن وحواف المكون الخارجي (البامبي) متساوية
    int uniformPadding = 20;

    // تطبيق الهامش الموحد على جميع الجوانب الأربعة
    auto area = getLocalBounds().reduced(uniformPadding);

    // ---------------------------------------------------------------------

    // إعدادات الأحجام
    int rowHeight = 20; // ارتفاع الأزرار 
    int smallSpacing = 5;

    // 2. الأزرار (الصف العلوي)
    auto buttonArea = area.removeFromTop(rowHeight + 10);
    int buttonWidth = buttonArea.getWidth() / 14;

    // توزيع 14 زر 
    loadButton.setBounds(buttonArea.removeFromLeft(buttonWidth));
    playButton.setBounds(buttonArea.removeFromLeft(buttonWidth));
    pauseButton.setBounds(buttonArea.removeFromLeft(buttonWidth));
    muteButton.setBounds(buttonArea.removeFromLeft(buttonWidth));
    restartButton.setBounds(buttonArea.removeFromLeft(buttonWidth));
    gotostartButton.setBounds(buttonArea.removeFromLeft(buttonWidth));
    endButton.setBounds(buttonArea.removeFromLeft(buttonWidth));
    stopButton.setBounds(buttonArea.removeFromLeft(buttonWidth));
    LoopButton.setBounds(buttonArea.removeFromLeft(buttonWidth));
    setAButton.setBounds(buttonArea.removeFromLeft(buttonWidth));
    setBButton.setBounds(buttonArea.removeFromLeft(buttonWidth));
    clearABButton.setBounds(buttonArea.removeFromLeft(buttonWidth));
    nextButton.setBounds(buttonArea.removeFromLeft(buttonWidth));
    previousButton.setBounds(buttonArea.removeFromLeft(buttonWidth));

    // ---------------------------------------------------------------------

    

    area.removeFromTop(smallSpacing); // مسافة فاصلة

    // Position Slider (شريط الموقع الرئيسي) - الذي أشرتِ إليه
    positionSlider.setBounds(area.removeFromTop(15));

    // Time Label (تم تقليصه سابقاً)
    timeLabel.setBounds(area.removeFromTop(15));

    area.removeFromTop(smallSpacing); // مسافة فاصلة

    // Volume and Speed Sliders (الآن تحت Time Label)
    auto slidersRowArea = area.removeFromTop(35);
    int halfWidth = slidersRowArea.getWidth() / 2;
    int labelWidth = 80;

    // A. Speed Slider (اليسار)
    auto speedArea = slidersRowArea.removeFromLeft(halfWidth);
    speedLabel.setBounds(speedArea.removeFromLeft(labelWidth));
    speedSlider.setBounds(speedArea);

    // B. Volume Slider (اليمين)
    auto volumeArea = slidersRowArea;
    volumeLabel.setBounds(volumeArea.removeFromLeft(labelWidth));
    volumeSlider.setBounds(volumeArea);

    area.removeFromTop(smallSpacing); // مسافة فاصلة

    // 4. Sleep Timer
    auto sleepArea = area.removeFromTop(35);

    int sleepButtonHeight = 25;

    sleepTimeEditor.setBounds(sleepArea.removeFromLeft(100).withHeight(sleepButtonHeight));
    startSleepButton.setBounds(sleepArea.removeFromLeft(160).withHeight(sleepButtonHeight));

    // ---------------------------------------------------------------------

    // 5. Progress Bar
    auto progressArea = area.removeFromTop(35);
    int timeWidth = 100;
    progressTimeLabal.setBounds(progressArea.getX(), progressArea.getY(), timeWidth, progressArea.getHeight());
    progressTimeLabal.setJustificationType(juce::Justification::centredRight);
    progressBar.setBounds(progressArea.getX() + timeWidth + 5, progressArea.getY(),
        progressArea.getWidth() - timeWidth - 5, progressArea.getHeight());

    // metadata label
    metadataLabel.setBounds(area.removeFromTop(24));

    // 6. قائمة التشغيل (Playlist Box) - تأخذ المساحة المتبقية
    playlistBox.setBounds(area);
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
   
    if (slider == &volumeSlider)
    {
        
       // playerAudio.setGain(volumeSlider.getValue());
        float internalGain = volumeSlider.getValue();
        playerAudio.setInternalVolume(internalGain);
        playerAudio.setGain(currentCrossfadeGain * internalGain);
    }
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


// الملف: PlayerGUI.cpp (تعديل دالة paint)

void PlayerGUI::paint(juce::Graphics& g)
{
    // 1. تعبئة خلفية PlayerGUI باللون الرمادي المتوسط 💡
    g.fillAll(juce::Colour(50, 50, 50)); // لون رمادي أفتح قليلاً من خلفية MainComponent

    // 2. رسم إطار حول PlayerGUI بنفس لون الإطار الرئيسي (البمبي الغامق) 💡
    juce::Colour orangeColour = juce::Colour::fromString("#FFB25200"); // اللون البرتقالي الداكن
    g.setColour(orangeColour);
    g.drawRect(getLocalBounds(), 2); // إطار بسمك 2 بكسل
}
