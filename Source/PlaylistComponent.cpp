

#include <JuceHeader.h>
#include "PlaylistComponent.h"

//==============================================================================
PlaylistComponent::PlaylistComponent(DeckGUI* _deckGUI1, 
                                     DeckGUI* _deckGUI2,
                                     DJAudioPlayer* _playerForParsingMetaData
                                    ) : deckGUI1(_deckGUI1),
                                        deckGUI2(_deckGUI2),
                                        playerForParsingMetaData(_playerForParsingMetaData)
{
    // In your constructor, you should add any child components, and
    // initialise any special settings that your component needs.
    
    // add components
    addAndMakeVisible(importButton);
    addAndMakeVisible(searchField);
    addAndMakeVisible(library);
    addAndMakeVisible(addToDeck1Button);
    addAndMakeVisible(addToDeck2Button);


    // buttons styling
    importButton.setColour(ComboBox::outlineColourId, Colours::deepskyblue);
    importButton.setColour(TextButton::textColourOffId, Colours::deepskyblue);
    addToDeck1Button.setColour(ComboBox::outlineColourId, Colours::deepskyblue);
    addToDeck1Button.setColour(TextButton::textColourOffId, Colours::deepskyblue);
    addToDeck2Button.setColour(ComboBox::outlineColourId, Colours::deepskyblue);
    addToDeck2Button.setColour(TextButton::textColourOffId, Colours::deepskyblue);

    // attach listeners
    importButton.addListener(this);
    searchField.addListener(this);
    addToDeck1Button.addListener(this);
    addToDeck2Button.addListener(this);

    // R3C searchField configuration
    searchField.setTextToShowWhenEmpty("Search track (Press enter to submit)", 
                                       juce::Colours::cyan);
    searchField.onReturnKey = [this] { searchLibrary (searchField.getText()); };
    
    // R3B setup table and load library from file
    library.getHeader().addColumn("Tracks", 1, 1);
    library.getHeader().addColumn("Length", 2, 1);
    library.getHeader().addColumn("X", 3, 1);
    library.setModel(this);

    //R3E 
    loadToLibrary();
}

PlaylistComponent::~PlaylistComponent()
{
    // R3E record the songs
    saveToLibrary();
}

void PlaylistComponent::paint (juce::Graphics& g)
{
    /* This demo code just fills the component's background and
       draws some placeholder text to get you started.

       You should replace everything in this method with your own
       drawing code..
    */

    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));   // clear the background
    // draw an outline around the component
    g.setColour (juce::Colours::black);
    g.drawRect (getLocalBounds(), 1);  

    // text color and font size
    g.setColour (juce::Colours::black);
    g.setFont (14.0f);

}

void PlaylistComponent::resized()
{
    // This method is where you should set the bounds of any child
    // components that your component contains..


    //                      (x start, y start, width, height)
    importButton.setBounds(0, 15 * getHeight() / 16, getWidth(), getHeight() / 16);
    library.setBounds(0, 1 * getHeight() / 16, getWidth(), 13 * getHeight() / 16);
    searchField.setBounds(0, 14 * getHeight() / 16, getWidth(), getHeight() / 16);
    addToDeck1Button.setBounds(0, 0, getWidth()/2, getHeight() / 16);
    addToDeck2Button.setBounds(getWidth() / 2, 0, getWidth() / 2, getHeight() / 16);  

    //set columns
    library.getHeader().setColumnWidth(1, 12.8 * getWidth() / 20);
    library.getHeader().setColumnWidth(2, 5 * getWidth() / 20);
    library.getHeader().setColumnWidth(3, 2 * getWidth() / 20);
}

// collect the number of rows which is the number of song in the library
int PlaylistComponent::getNumRows()
{
    return tracks.size();
}

// highlight the selected song
void PlaylistComponent::paintRowBackground(juce::Graphics& g,
                                             int rowNumber,
                                           int width,
                                           int height,
                                           bool rowIsSelected
                                          )
{
    if (rowIsSelected)
    {
        g.fillAll(juce::Colours::cyan);
    }
    else
    {
        g.fillAll(juce::Colours::darkgrey);
    }
}


void PlaylistComponent::paintCell(juce::Graphics& g,
                                  int rowNumber,
                                  int columnId,
                                  int width,
                                  int height,
                                  bool rowIsSelected
                                 )
{
// R3B display the song/track name and their length
    if (rowNumber < getNumRows())
    {
        if (columnId == 1)
        {
            g.drawText(tracks[rowNumber].title,
                2,
                0,
                width - 4,
                height,
                juce::Justification::centredLeft,
                true
            );
        }
        if (columnId == 2)
        {
            g.drawText(tracks[rowNumber].length,
                2,
                0,
                width - 4,
                height,
                juce::Justification::centred,
                true
            );
        }
    }
}

// delete the selected song using an X button
juce::Component* PlaylistComponent::refreshComponentForCell(int rowNumber,
                                                      int columnId,
                                                      bool isRowSelected,
                                                      Component* existingComponentToUpdate)
{
    if (columnId == 3)
    {
        if (existingComponentToUpdate == nullptr)
        {
            // create X button and link it to each song index
            juce::TextButton* btn = new juce::TextButton{ "X" };
            juce::String id{ std::to_string(rowNumber) };
            btn->setComponentID(id);
            btn->addListener(this);
            existingComponentToUpdate = btn;
        }
    }
    return existingComponentToUpdate;
}

void PlaylistComponent::buttonClicked(juce::Button* button)
{
    // R3A add song to library when import button is used
    if (button == &importButton)
    {
        DBG("Load button clicked");
        importToLibrary();
        //update the library
        library.updateContent();
    }
    // R3D load the song into the chosen Deck
    else if (button == &addToDeck1Button)
    {
        DBG("Add to Deck 1 clicked");
        loadInDeck(deckGUI1);
    }
    else if (button == &addToDeck2Button)
    {
        DBG("Add to Deck 2 clicked");
        loadInDeck(deckGUI2);
    }
    else
    {
        // remove the song from library
        int id = std::stoi(button->getComponentID().toStdString());
        DBG(tracks[id].title + " removed from Library");
        deleteSongs(id);
        // update the library
        library.updateContent();
    }
}

// R3D load the song into the chosen deck 
void PlaylistComponent::loadInDeck(DeckGUI* deckGUI)
{
    // identify the selected song by user
    int selectedRow{ library.getSelectedRow() };
    if (selectedRow != -1)
    {
        // load the chosen song to the deck
        DBG("Adding: " << tracks[selectedRow].title << " to Player");
        deckGUI->loadFile(tracks[selectedRow].URL);
    }
    else
    {
        // display message to remind user they need to choose a song
        juce::AlertWindow::showMessageBox(juce::AlertWindow::AlertIconType::InfoIcon,
            "Add to Deck Information:",
            "Please select a track to add to deck",
            "OK",
            nullptr
        );
    }
}

// R3A load the song file to the library
void PlaylistComponent::importToLibrary()
{
    DBG("PlaylistComponent::importToLibrary called");

    //initialize file chooser
    juce::FileChooser chooser{ "Select files" };
    if (chooser.browseForMultipleFilesToOpen())
    {
        for (const juce::File& file : chooser.getResults())
        {
            // parse the file name to obtain the name of the song
            juce::String fileName{ file.getFileNameWithoutExtension() };
            // if not already loaded then add into library
            if (!isInPlaylist(fileName)) 
            {
                // parse the file data
                Song newSong{ file };
                juce::URL audioURL{ file };
                newSong.length = getLength(audioURL) ;
                //add the song data to library
                tracks.push_back(newSong);
                DBG("loaded file: " << newSong.title);
            }
            else // display message when theres a replica of the song to inform users.
            {
                juce::AlertWindow::showMessageBox(juce::AlertWindow::AlertIconType::InfoIcon,
                    "Load information:",
                    fileName + " already loaded",
                    "OK",
                    nullptr
                );
            }
        }
    }
}

// R3A compare the names inside the playlist and return true when theres a exact copy to ensure theres no replicates
bool PlaylistComponent::isInPlaylist(juce::String fileName)
{
    return (std::find(tracks.begin(), tracks.end(), fileName) != tracks.end());
}

// remove the song form playlist
void PlaylistComponent::deleteSongs(int id)
{
    tracks.erase(tracks.begin() + id);
}

// R3B get the length of the song
juce::String PlaylistComponent::getLength(juce::URL audioURL)
{
    playerForParsingMetaData->loadURL(audioURL);
    // the full song length in seconds
    double seconds{ playerForParsingMetaData->getLengthInSeconds() };
    // record the string
    juce::String minutes{ secondsToMinutes(seconds) };
    return minutes;
}
juce::String PlaylistComponent::secondsToMinutes(double seconds)
{
    //find seconds and minutes and make into string
    int secondsRounded{ int(std::round(seconds)) };
    juce::String min{ std::to_string(secondsRounded / 60) };
    juce::String sec{ std::to_string(secondsRounded % 60) };
    
    if (sec.length() < 2) // if seconds is 1 digit or less
    {
        //add '0' to seconds until seconds is length 2
        sec = sec.paddedLeft('0', 2);
    }
    return juce::String{ min + ":" + sec };
}

// R3C using searchbox to allow users find the song they desire
void PlaylistComponent::searchLibrary(juce::String searchText)
{
    DBG("Searching library for: " << searchText); 

    if (searchText != "")
    {
        // using whereInPLaylist to obtain the index of the song which allow us to choose which row it is by using selectRow
        int rowNumber = whereInPlaylist(searchText);
        library.selectRow(rowNumber);
    }
    else
    {
        // when it is blank, deselect everything
        library.deselectAllRows();
    }
}

// R3C identify the poistion of the song in the playlist by using the search text
int PlaylistComponent::whereInPlaylist(juce::String searchText)
{
    // finds index where track title contains searchText
    // it is case insensitive
    auto chosenSong = find_if(tracks.begin(), tracks.end(), 
        [&searchText](const Song& obj) 
        {
            return obj.title.containsIgnoreCase(searchText); 
        });
    int i = -1;

    if (chosenSong != tracks.end())
    {
        i = std::distance(tracks.begin(), chosenSong);
    }

    return i;
}

// R3E store the data of the library locally to allow the library to persist
void PlaylistComponent::saveToLibrary()
{
    // create .csv to save library
    std::ofstream my_Library("my_library.csv");

    // save library to file
    for (Song& t : tracks)
    {
        my_Library << t.file.getFullPathName() << "," << t.length << "\n";
    }
}

//R3E load the saved library to the current one. Allowing the program to "remember" what songs were added
void PlaylistComponent::loadToLibrary()
{
    // create input stream from saved library
    std::ifstream my_Library("my_library.csv");
    std::string filePath;
    std::string length;

    // Read data, line by line
    if (my_Library.is_open())
    {
        // add each songs found in the .csv to the library
        while (getline(my_Library, filePath, ',')) {
            juce::File file{ filePath };
            Song newSong{ file };

            getline(my_Library, length);
            newSong.length = length;
            tracks.push_back(newSong);
        }
    }
    my_Library.close();
}