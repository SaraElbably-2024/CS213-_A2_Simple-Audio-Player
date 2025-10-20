#include"PlayerAudio.h"
PlayerAudio::PlayerAudio()
{
	formatManager.registerBasicFormats();
}
PlayerAudio::~PlayerAudio()
{
    releaseResources();
}
void PlayerAudio ::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
	transportSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
}
void PlayerAudio ::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
	transportSource.getNextAudioBlock(bufferToFill);
}
void PlayerAudio ::releaseResources()
{
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

            // Create new reader source
            readerSource = std::make_unique<juce::AudioFormatReaderSource>(reader, true);

            // Attach safely
            transportSource.setSource(readerSource.get(),
                0,
                nullptr,
                reader->sampleRate);
            transportSource.start();
			return true; // Successfully loaded the file
        }
		return false; // Failed to create reader for the file

    }
    return false;
}
void PlayerAudio::play()
{
    transportSource.start();
}
void PlayerAudio::stop()
{
    transportSource.stop();
	transportSource.setPosition(0.0); // Reset position to start
}
void PlayerAudio::setGain(float gain)
{
    transportSource.setGain(gain);
}
void PlayerAudio::setPosition(double position)
{
    transportSource.setPosition(position);
}
//void PlayerAudio::setLooping(bool shouldLoop)
//{
//    transportSource.setLooping(shouldLoop);
//}
double PlayerAudio::getPosition() const
{
      
   return transportSource.getCurrentPosition();
}
double PlayerAudio::getLength()const
{
      return transportSource.getLengthInSeconds();
}
 


