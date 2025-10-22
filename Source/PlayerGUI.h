#pragma once
#include <JuceHeader.h>
#include "PlayerAudio.h"
class PlayerGUI : public juce::Component,
    public juce::Button::Listener,
    public juce::Slider::Listener
{
public:
    PlayerGUI();
    ~PlayerGUI() override;

    void resized() override;

    void prepareToPlay(int samplesPerBlockExpected, double sampleRate);
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill);
    void releaseResources();
private:
    PlayerAudio playerAudio;
    //GUI components
    juce::TextButton playButton{ "Play" };
    juce::TextButton stopButton{ "Stop" };
    juce::TextButton loadButton{ "Load" };
    juce::TextButton muteButton{ "Mute" };
    juce::TextButton restartButton{ "Restart" };
    juce::TextButton LoopButton{ "Loop OFF" };
    juce::Slider volumeSlider;

    std::unique_ptr<juce::FileChooser> fileChooser;

    bool muted = false;
    float previousVolume = 0.05f;
    bool isLooping = false;
    //Event handlers
    void buttonClicked(juce::Button* button) override;
    void sliderValueChanged(juce::Slider* slider) override;


    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlayerGUI)

};