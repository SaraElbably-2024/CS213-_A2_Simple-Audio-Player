#include "PlayerAudio.h"

PlayerAudio::PlayerAudio()
{
    formatManager.registerBasicFormats();

    //speed====
    resampleSource = std::make_unique<juce::ResamplingAudioSource>(&transportSource, false);

}

PlayerAudio::~PlayerAudio()
{
    releaseResources();
}

void PlayerAudio::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    transportSource.prepareToPlay(samplesPerBlockExpected, sampleRate);

    //speed====
    resampleSource->prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void PlayerAudio::releaseResources()
{
    //speed====
    resampleSource->releaseResources();
    transportSource.releaseResources();
}

bool PlayerAudio::loadFile(const juce::File& file)
{
    if (file.existsAsFile())
    {
        if (auto* reader = formatManager.createReaderFor(file))
        {
            transportSource.stop();
            transportSource.setSource(nullptr);
            readerSource.reset();

            readerSource = std::make_unique<juce::AudioFormatReaderSource>(reader, true);

            transportSource.setSource(readerSource.get(), 0, nullptr, reader->sampleRate);
            transportSource.start();
            return true;
        }
        return false;
    }
    return false;
}

void PlayerAudio::play() { transportSource.start(); }
void PlayerAudio::stop() { transportSource.stop(); }
void PlayerAudio::pause() { transportSource.stop(); }
void PlayerAudio::gotostart() { transportSource.setPosition(0.0); }
void PlayerAudio::setGain(float gain) { transportSource.setGain(gain); }
void PlayerAudio::setPosition(double position) { transportSource.setPosition(position); }
void PlayerAudio::restart() { transportSource.setPosition(0.0); transportSource.start(); }
void PlayerAudio::setLooping(bool shouldLoop) { isLooping = shouldLoop; }

double PlayerAudio::getPosition() const { return transportSource.getCurrentPosition(); }
double PlayerAudio::getLength() const { return transportSource.getLengthInSeconds(); }

void PlayerAudio::setSegmentLoop(double start, double end)
{
    if (start >= 0.0 && end > start && end <= transportSource.getLengthInSeconds()) {
        segmentStart = start;
        segmentEnd = end;
        isLooping = false;
        DBG("Segment loop set from " + juce::String(start) + " to " + juce::String(end));
    }
    else if (start == -1.0 && end == -1.0) {
        segmentStart = -1.0;
        segmentEnd = -1.0;
        DBG("Segment loop cleared");
    }
}

void PlayerAudio::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    //speed====
    resampleSource->getNextAudioBlock(bufferToFill);

    double currentPos = transportSource.getCurrentPosition();

    if (segmentStart != -1.0 && segmentEnd != -1.0)
    {
        if (currentPos >= segmentEnd)
            transportSource.setPosition(segmentStart);
    }
    else if (isLooping && currentPos >= transportSource.getLengthInSeconds())
    {
        transportSource.setPosition(0.0);
        transportSource.start();
    }
}

//speed====
void PlayerAudio::setSpeed(double newSpeed)
{
    if (newSpeed > 0.0)
    {
        currentSpeed = newSpeed;
        resampleSource->setResamplingRatio(currentSpeed);
    }
}
//sleeptime
void PlayerAudio::startSleepTimer(double seconds)
{
    sleepTimerEnabled = true;
    sleepTimeInSeconds = seconds;
    sleepStartTime = juce::Time::getCurrentTime();
}
float PlayerAudio::getGain() const
{
    return transportSource.getGain();
}

void PlayerAudio::checkSleepTimer()
{
    if (sleepTimerEnabled)
    {
        auto elapsed = (juce::Time::getCurrentTime() - sleepStartTime).inSeconds();
        if (elapsed >= sleepTimeInSeconds)
        {
            transportSource.stop();
            sleepTimerEnabled = false;
        }
    }
}
