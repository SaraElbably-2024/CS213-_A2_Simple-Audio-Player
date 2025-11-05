#pragma once

#include <JuceHeader.h>

class PlayerAudio : public juce::AudioSource
{
public:
    PlayerAudio();
    ~PlayerAudio() override;

    // juce::AudioSource overrides
    void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
    void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;
    void releaseResources() override;
    void setSegmentLoop(double start, double end);

    // Player controls
    bool loadFile(const juce::File& file);
    void play();
    void stop();
    void pause();
    void gotostart();
    void setGain(float gain);
    float getGain()const;
    void setInternalVolume(float gain);
    void setPosition(double position);
    void restart();
    void setLooping(bool shouldLoop);
    void playNextInPlaylist();
    void playPreviousInPlaylist();
	
    //speed====
    void setSpeed(double newSpeed); // New function to control playback speed

    // Query methods
    double getPosition() const;
    double getLength() const;
    juce::String getFileInfo()const;

    //sleeptime
    void startSleepTimer(double seconds);
    void checkSleepTimer();
private:
    juce::AudioFormatManager formatManager;
    juce::AudioTransportSource transportSource;
    std::unique_ptr<juce::AudioFormatReaderSource> readerSource;
    
    //speed====
    std::unique_ptr<juce::ResamplingAudioSource> resampleSource; // will wrap transportSource
    
    bool isLooping = false;
    double segmentStart = -1.0;
    double segmentEnd = -1.0;

    double currentSpeed = 1.0; //speed==== default speed

    //sleeptime
    bool sleepTimerEnabled = false;
    double sleepTimeInSeconds = 0;
    juce::Time sleepStartTime;
    //---crossfader------
    float internalVolumeGain = 1.0f;

    //fileinformation
    juce::String fileInfo;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlayerAudio)

};
