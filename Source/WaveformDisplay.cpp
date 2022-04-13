
#include <JuceHeader.h>
#include "WaveformDisplay.h"

//==============================================================================
WaveformDisplay::WaveformDisplay(int _id,
                                 juce::AudioFormatManager& formatManager,
                                 juce::AudioThumbnailCache& thumbCache
                                ) : audioThumb(1000, formatManager, thumbCache),
                                    fileLoaded(false),
                                    position(0),
                                    id(_id)
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.
    audioThumb.addChangeListener(this);
}

WaveformDisplay::~WaveformDisplay()
{
}

void WaveformDisplay::paint (juce::Graphics& g)
{

    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId)); // clear the background
    
    // draw an outline around the component
    g.setColour (juce::Colours::black);
    g.drawRect(getLocalBounds(), 1);  

    g.setColour(juce::Colours::cyan);
    g.setFont(18.0f);
    g.drawText("Deck " + std::to_string(id), getLocalBounds(),
            juce::Justification::centredTop, true);


    if (fileLoaded)
    {
        // when file is loaded, waveform will be drawn
        g.setFont(15.0f);
        audioThumb.drawChannel(g, 
                               getLocalBounds(),
                               0,
                               audioThumb.getTotalLength(),
                               0,
                               1.0f
                              );
        g.setColour(juce::Colours::red);
        g.drawRect(position * getWidth(), 0, getWidth() / 20, getHeight());
        g.setColour(juce::Colours::pink);
        g.drawText(fileName, getLocalBounds(),
            juce::Justification::bottomLeft, true);
    }
    else
    {
        // otherwise placeholder text will be shown
        g.setFont(20.0f);
        g.drawText("File not loaded...", getLocalBounds(),
            juce::Justification::centred, true);   // draw some placeholder text
    }
}

void WaveformDisplay::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..

}

void WaveformDisplay::changeListenerCallback(juce::ChangeBroadcaster* source)
{
    repaint();
}

void WaveformDisplay::loadURL(juce::URL audioURL)
{
    DBG("WaveformDisplay::loadURL called");
    audioThumb.clear();
    fileLoaded = audioThumb.setSource(new juce::URLInputSource(audioURL));
    if (fileLoaded)
    {
        DBG("WaveformDisplay::loadURL file loaded");
        fileName = audioURL.getFileName();
        repaint();
    }
    else
    {
        DBG("WaveformDisplay::loadURL file NOT loaded");
    }
}

void WaveformDisplay::setPositionRelative(double pos)
{
    if (pos != position && !isnan(pos))
    {
        position = pos;
        repaint();
    }
}
