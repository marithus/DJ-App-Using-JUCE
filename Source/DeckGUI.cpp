
#include "DeckGUI.h"

//==============================================================================
DeckGUI::DeckGUI(int _id,
                 DJAudioPlayer* _player, 
                 juce::AudioFormatManager& formatManager,
                 juce::AudioThumbnailCache& thumbCache
                ) : player(_player),
                    id(_id),
                    waveformDisplay(id, formatManager, thumbCache)
{
    // add all components and make visible
    addAndMakeVisible(playButton);
    addAndMakeVisible(stopButton);
    addAndMakeVisible(loadButton);
    addAndMakeVisible(loopButton);

    addAndMakeVisible(volSlider);
    addAndMakeVisible(volLabel);
    addAndMakeVisible(speedSlider);
    addAndMakeVisible(speedLabel);
    addAndMakeVisible(posSlider);
    addAndMakeVisible(posLabel);

    addAndMakeVisible(reverbGraph1);
    addAndMakeVisible(reverbGraph2);

    addAndMakeVisible(waveformDisplay);


    // buttons styling
    playButton.setColour(ComboBox::outlineColourId, Colours::deepskyblue);
    playButton.setColour(TextButton::textColourOffId, Colours::deepskyblue);
    stopButton.setColour(ComboBox::outlineColourId, Colours::deepskyblue);
    stopButton.setColour(TextButton::textColourOffId, Colours::deepskyblue);
    loadButton.setColour(ComboBox::outlineColourId, Colours::deepskyblue);
    loadButton.setColour(TextButton::textColourOffId, Colours::deepskyblue);

    // loop button styling when toggled
    loopButton.setColour(ComboBox::outlineColourId, Colours::deepskyblue);
    loopButton.setColour(TextButton::textColourOffId, Colours::deepskyblue);
    loopButton.setColour(TextButton::textColourOnId, Colours::limegreen);

    // add listeners
    playButton.addListener(this);
    stopButton.addListener(this);
    loadButton.addListener(this);
    volSlider.addListener(this);
    speedSlider.addListener(this);
    posSlider.addListener(this);
    reverbSlider.addListener(this);
    reverbGraph1.addListener(this);
    reverbGraph2.addListener(this);
    loopButton.addListener(this);


    //configure volume slider and label
    double volDefaultValue = 0.5;
    volSlider.setRange(0.0, 1.0);
    volSlider.setNumDecimalPlacesToDisplay(2);
    volSlider.setTextBoxStyle(juce::Slider::TextBoxLeft, 
                              false, 
                              50, 
                              volSlider.getTextBoxHeight()
                             );
    volSlider.setValue(volDefaultValue);
    volSlider.setSkewFactorFromMidPoint(volDefaultValue);
    volLabel.setText("Volume", juce::dontSendNotification);
    volLabel.attachToComponent(&volSlider, true);

    //configure speed slider and label
    double speedDefaultValue = 1.0;
    speedSlider.setRange(0.25, 4.0); //reaches breakpoint if sliderValue == 0
    speedSlider.setNumDecimalPlacesToDisplay(2);
    speedSlider.setTextBoxStyle(juce::Slider::TextBoxLeft,
                              false,
                              50,
                              speedSlider.getTextBoxHeight()
                             );
    speedSlider.setValue(speedDefaultValue);
    speedSlider.setSkewFactorFromMidPoint(speedDefaultValue);
    speedLabel.setText("Speed", juce::dontSendNotification);
    speedLabel.attachToComponent(&speedSlider, true);

    //configure position slider and label
    posSlider.setRange(0.0, 1.0);
    posSlider.setNumDecimalPlacesToDisplay(2);
    posSlider.setTextBoxStyle(juce::Slider::TextBoxLeft,
                              false,
                              50,
                              posSlider.getTextBoxHeight()
                             );
    posLabel.setText("Position", juce::dontSendNotification);
    posLabel.attachToComponent(&posSlider, true);

    //configure reverb slider
    reverbSlider.setRange(0.0, 1.0);
    reverbSlider.setNumDecimalPlacesToDisplay(2);
    //Notation to help users on functions
    reverbGraph1.setTooltip("x: Damping Level\ny: Spatial Level");
    reverbGraph2.setTooltip("x: Dry level\ny: Wet level");

    startTimer(500);
}

DeckGUI::~DeckGUI()
{
    stopTimer();
}

void DeckGUI::paint (juce::Graphics& g)
{
    /* This demo code just fills the component's background and
       draws some placeholder text to get you started.

       You should replace everything in this method with your own
       drawing code..
    */

    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));   // clear the background

    // draw an outline around the component
    g.setColour (juce::Colours::grey);
    g.drawRect (getLocalBounds(), 1);   

    //fill the DeckGUI with color
    g.setColour(Colours::black);
    g.fillRoundedRectangle(0, 0, getWidth(), getHeight(), 3.0f);


}

void DeckGUI::resized()
{
     /*This method is where you should set the bounds of any child
     components that your component contains..*/
    auto sliderPos = getWidth() / 9;
    auto mainPos = getWidth() - getHeight() / 2;
    auto graphPos = getWidth() - mainPos;


    //position of everything
    //(x start, y start, width, height)

    //buttons position
    playButton.setBounds(0, 0, mainPos / 4, getHeight() / 8);
    stopButton.setBounds(mainPos / 4, 0, mainPos / 4, getHeight() / 8);
    loadButton.setBounds(2 * mainPos / 4, 0, mainPos / 4, getHeight() / 8);
    loopButton.setBounds(3 * mainPos / 4, 0, mainPos / 4, getHeight() / 8);

    // sliders position
    volSlider.setBounds(sliderPos, getHeight() / 8, mainPos - sliderPos, getHeight() / 8);
    speedSlider.setBounds(sliderPos, 2 * getHeight() / 8, mainPos - sliderPos, getHeight() / 8);
    posSlider.setBounds(sliderPos, 3 * getHeight() / 8, mainPos - sliderPos, getHeight() / 8);

    waveformDisplay.setBounds(0, 4 * getHeight() / 8, mainPos, 4 * getHeight() / 8);

    reverbGraph1.setBounds(mainPos, 0, graphPos, getHeight() / 2);
    reverbGraph2.setBounds(mainPos, getHeight()/2, graphPos, getHeight() / 2);

}

//R2A
void DeckGUI::buttonClicked(juce::Button* button)
{
    if (button == &playButton)
    {
        DBG("Play button was clicked ");
        player->play();
    }
    if (button == &stopButton)
    {
        DBG("Stop button was clicked ");
        player->stop();
    }
    //R1A
    if (button == &loadButton)
    {
        DBG("Load button was clicked ");
        juce::FileChooser chooser{"Select a file"};
        // user can choose specifically which files to load in
        if (chooser.browseForFileToOpen())
        {
            loadFile(juce::URL{ chooser.getResult() });
        }
    }
    // toggle loop to change loop state to true on first click while change it to false on second click
    if (button == &loopButton)
    {
        DBG("Loop button was clicked ");
        loopButton.setToggleState(!loopButton.getToggleState(), dontSendNotification);
        player->looping = !player->looping;
    }
}

// all sliders change will affect the values assigned to them
void DeckGUI::sliderValueChanged(juce::Slider* slider)
{
    // change the volume of the song when slider move
    if (slider == &volSlider)
    {
        DBG("Volume slider moved " << slider->getValue());
        player->setGain(slider->getValue());
    }
    //change the speed of the song when slider move
    if (slider == &speedSlider)
    {
        DBG("Speed slider moved " << slider->getValue());
        player->setSpeed(slider->getValue());
    }
    //move to the chosen position by users
    if (slider == &posSlider)
    {
        DBG("Position slider moved " << slider->getValue());
        player->setPositionRelative(slider->getValue());
    }
}

// reverb graphs changed will affect the values set to them
void DeckGUI::graphValueChange(graphDisplay* graphPoints)
{
    DBG("DeckGUI::graphValueChange called");
    if (graphPoints == &reverbGraph1)
    {
        DBG("Deck " << id << ": ReverbPlot1 was clicked");
        player->setRoomSize(graphPoints->getY());
        player->setDamping(graphPoints->getX());
    }
    if (graphPoints == &reverbGraph2)
    {
        DBG("Deck " << id << ": ReverbPlot2 was clicked");
        player->setWetLevel(graphPoints->getY());
        player->setDryLevel(graphPoints->getX());
    }
}


//R1A Drag and drop files to deck
// detect when user drag a file onto deckGUI
bool DeckGUI::isInterestedInFileDrag(const juce::StringArray& files)
{
    DBG("DeckGUI::isInterestedInFileDrag called. " 
        + std::to_string(files.size()) + " file(s) being dragged.");
    return true;
}
// detect when user drop the mentioned file onto deckGUI and load it into the deck using loadFile
void DeckGUI::filesDropped(const juce::StringArray& files, int x, int y)
{

    DBG("DeckGUI::filesDropped at " + std::to_string(x) 
        + "x and " + std::to_string(y) + "y" );
    if (files.size() == 1)
    {
        loadFile(juce::URL{ juce::File{files[0]} });
    }
}
// function to load the chosen file into the deck
void DeckGUI::loadFile(juce::URL audioURL)
{
    DBG("DeckGUI::loadFile called");
    player->loadURL(audioURL);
    waveformDisplay.loadURL(audioURL);
}

void DeckGUI::timerCallback()
{   
    //check if the relative position is greater than 0  
    //otherwise loading file causes error
    if (player->getPositionRelative() > 0)
    {
        waveformDisplay.setPositionRelative(player->getPositionRelative());
        //loop the audio and set it back to the start
        if (player->looping == true)
        {
            player->loop(0);
        }
    }

}