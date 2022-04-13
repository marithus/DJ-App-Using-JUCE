

#pragma once
#include "../JuceLibraryCode/JuceHeader.h"

class DJAudioPlayer : public juce::AudioSource
{
    public:
        DJAudioPlayer(juce::AudioFormatManager& _formatManager);
        ~DJAudioPlayer();

        void prepareToPlay(int samplesPerBlockExpected, double sampleRate) override;
        void getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override;
        void releaseResources() override;

        /**Loads the audio file*/
        void loadURL(juce::URL audioURL);
        /**Plays loaded audio file*/
        void play();
        /**Stops playing audio file*/
        void stop();
        /**Sets relative position of audio file*/
        void setPositionRelative(double pos);
        /**Sets the volume*/
        void setGain(double gain);
        /**Sets the speed*/
        void setSpeed(double ratio);
        /**Gets relative position of playhead*/
        double getPositionRelative();
        /**Gets the length of transport source in seconds*/
        double getLengthInSeconds();
        /**Control the amount of reverb*/
        void setRoomSize(float roomSizeLevel);
        void setDamping(float dampingLevel);
        void setWetLevel(float wetLevel);
        void setDryLevel(float dryLevel);
        /**Loop the audio file**/
        void loop(double pos);
        bool looping;

    private:
        void setPosition(double posInSecs);
        juce::AudioFormatManager& formatManager;
        std::unique_ptr<juce::AudioFormatReaderSource> readerSource;
        juce::AudioTransportSource transportSource;
        juce::ResamplingAudioSource resampleSource{ &transportSource, false, 2 };
        juce::ReverbAudioSource reverbSource{ &resampleSource, false };
        juce::Reverb::Parameters reverbParameters;
};