#pragma once
#include <JuceHeader.h>
#include "PlayerAudio.h"
class PlayerGUI : public juce::Component,
    public juce::Button::Listener,
    public juce::Slider::Listener,
    public juce::Timer,
    public juce::AudioSource,
    public juce::ListBoxModel
{
public:
    PlayerGUI();
    ~PlayerGUI() override;

    void resized() override;
    void timerCallback() override;
    void paint(juce::Graphics& g) override;
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate);
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill);
    void releaseResources();
    void setGain(float gain);

    //amira part
    int getNumRows() override;
    void paintListBoxItem(int rowNumber, juce::Graphics& g, int width, int height, bool rowIsSelected) override;
    void listBoxItemClicked(int row, const juce::MouseEvent& e) override;
    void updateMetadataLabel();

private:
    PlayerAudio playerAudio;
    //GUI components
    juce::TextButton playButton{ "Play" };
    juce::TextButton stopButton{ "Stop" };
    juce::TextButton loadButton{ "Load" };
    juce::TextButton muteButton{ "Mute" };
    juce::TextButton restartButton{ "Restart" };
    juce::TextButton gotostartButton{ "Go to start" };
    juce::TextButton pauseButton{ "Pause" };
    juce::TextButton endButton{ "End" };
    juce::TextButton LoopButton{ "Loop OFF" };
    juce::TextButton nextButton{ "Next" };
    juce::TextButton previousButton{ " Previous" };
    //label and playlistbox
    juce::Label metadataLabel;
    juce::ListBox playlistBox;
   

    juce::Slider volumeSlider;
    juce::Slider positionSlider;
    juce::Label timeLabel;
    //new by bably
    juce::Slider speedSlider;
    juce::Label speedLabel;
    //======
    juce::TextButton setAButton{ "Set A" };
    juce::TextButton setBButton{ "Set B" };
    juce::TextButton clearABButton{ "Clear A-B" };
    //sleeptime
    juce::TextEditor sleepTimeEditor;
    juce::TextButton startSleepButton{ "Start Sleep" };
    //--------
    std::unique_ptr<juce::FileChooser> fileChooser;
    //slider progress bar
    juce::Slider progressBar;
    juce::Label progressTimeLabal;
    //--------
    juce::Array<juce::File> playlistFiles;
    bool muted = false;
    float previousVolume = 0.05f;
    bool isLooping = false;
    double loopStartA = -1.0;
    double loopEndB = -1.0;
    int currentIndex =-1;

    //Event handlers
    void buttonClicked(juce::Button* button) override;
    void sliderValueChanged(juce::Slider* slider) override;
    juce::String secondsToTimeString(double seconds);
    
    void playFileAtIndex(int index);
    void playNextInPlaylist();
    void playPreviousInPlaylist();



    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlayerGUI)

};
