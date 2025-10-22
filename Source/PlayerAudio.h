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

    // Player controls
    bool loadFile(const juce::File& file);
    void play();
    void stop();
    void setGain(float gain);
    void setPosition(double position);
    void restart();
    void setLooping(bool shouldLoop);

    // Query methods (match implementation: return double)
    double getPosition() const;
    double getLength() const;

private:
    juce::AudioFormatManager formatManager;
    juce::AudioTransportSource transportSource;
    std::unique_ptr<juce::AudioFormatReaderSource> readerSource;
    bool isLooping = false;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PlayerAudio)
};