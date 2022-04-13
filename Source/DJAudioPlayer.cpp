
#include "DJAudioPlayer.h"
DJAudioPlayer::DJAudioPlayer(juce::AudioFormatManager& _formatManager
                            ) : formatManager(_formatManager) , looping(false)
{
    //Default reverb settings
    reverbParameters.roomSize = 0;
    reverbParameters.damping = 0;
    reverbParameters.wetLevel = 0;
    reverbParameters.dryLevel = 1.0;
    reverbSource.setParameters(reverbParameters);
}

DJAudioPlayer::~DJAudioPlayer()
{
}

void DJAudioPlayer::prepareToPlay(int samplesPerBlockExpected, double sampleRate)
{
    transportSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
    resampleSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
    reverbSource.prepareToPlay(samplesPerBlockExpected, sampleRate);
}

void DJAudioPlayer::getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill)
{
    reverbSource.getNextAudioBlock(bufferToFill);
}

void DJAudioPlayer::releaseResources()
{
    transportSource.releaseResources();
    resampleSource.releaseResources();
    reverbSource.releaseResources();
}

// R1A
void DJAudioPlayer::loadURL(juce::URL audioURL)
{
    DBG("DJAudioPlayer::loadURL called");
    auto* reader = formatManager.createReaderFor(audioURL.createInputStream(false));
    if (reader != nullptr) // good file!
    {
        std::unique_ptr<juce::AudioFormatReaderSource> newSource(new juce::AudioFormatReaderSource(reader,
            true));
        transportSource.setSource(newSource.get(), 0, nullptr, reader->sampleRate);
        readerSource.reset(newSource.release());
    }
}

// R2A Start the song
void DJAudioPlayer::play()
{
    transportSource.start();
}

// stop the song
void DJAudioPlayer::stop()
{
    transportSource.stop();
}

// R2B loop the song by reseting the position back to the start where pos = 0
void DJAudioPlayer::loop(double pos)
{
    if (transportSource.getCurrentPosition() >= transportSource.getLengthInSeconds() - 2)
    {
        transportSource.setPosition(transportSource.getLengthInSeconds() * pos);
    }
}

void DJAudioPlayer::setPosition(double posInSecs)
{
    transportSource.setPosition(posInSecs);
}

//R2A allow user select the position in the song
void DJAudioPlayer::setPositionRelative(double pos)
{
    if (pos < 0 || pos > 1.0)
    {
        DBG("DJAudioPlayer::setPositionRelative position should be between 0 and 1");
    }
    else {
        double posInSecs = transportSource.getLengthInSeconds() * pos;
        setPosition(posInSecs);
    }
}

// R1C volume of the song
void DJAudioPlayer::setGain(double gain)
{
    if (gain < 0 || gain > 1.0)
    {
        DBG("DJAudioPlayer::setGain gain should be between 0 and 1");
    }
    else {
        transportSource.setGain(gain);
    }
}

// R1D speed of the song
void DJAudioPlayer::setSpeed(double ratio)
{
    if (ratio < 0.25 || ratio > 4.0)
    {
        DBG("DJAudioPlayer::setSpeed ratio should be between 0.25 and 4");
    }
    else {
        resampleSource.setResamplingRatio(ratio);
    }
}


// functions that will change the reverbs by using the JUCE library
// change the roomsize of the song
void DJAudioPlayer::setRoomSize(float roomSizeLevel)
{
    DBG("DJAudioPlayer::setRoomSize called");
    if (roomSizeLevel < 0 || roomSizeLevel > 1.0)
    {
        DBG("DJAudioPlayer::setRoomSize size should be between 0 and 1.0");
    }
    else {
        reverbParameters.roomSize = roomSizeLevel;
        reverbSource.setParameters(reverbParameters);
    }
}
//change the damping value of the song
void DJAudioPlayer::setDamping(float dampingLevel)
{
    DBG("DJAudioPlayer::setDamping called");
    if (dampingLevel < 0 || dampingLevel > 1.0)
    {
        DBG("DJAudioPlayer::setDamping amount should be between 0 and 1.0");
    }
    else {
        reverbParameters.damping = dampingLevel;
        reverbSource.setParameters(reverbParameters);
    }
}
// change wet level of the song
void DJAudioPlayer::setWetLevel(float wetLevel)
{
    DBG("DJAudioPlayer::setWetLevel called");
    if (wetLevel < 0 || wetLevel > 1.0)
    {
        DBG("DJAudioPlayer::setWetLevel level should be between 0 and 1.0");
    }
    else {
        reverbParameters.wetLevel = wetLevel;
        reverbSource.setParameters(reverbParameters);
    }
}
// change the drylevel of the song
void DJAudioPlayer::setDryLevel(float dryLevel)
{
    DBG("DJAudioPlayer::setDryLevel called");
    if (dryLevel < 0 || dryLevel > 1.0)
    {
        DBG("DJAudioPlayer::setDryLevel level should be between 0 and 1.0");
    }
    else {
        reverbParameters.dryLevel = dryLevel;
        reverbSource.setParameters(reverbParameters);
    }
}

double DJAudioPlayer::getPositionRelative()
{
    return transportSource.getCurrentPosition() / transportSource.getLengthInSeconds();
}

double DJAudioPlayer::getLengthInSeconds()
{
    return transportSource.getLengthInSeconds();
}
