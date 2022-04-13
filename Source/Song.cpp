

#include "Song.h"
#include <filesystem>

//R3B parse the data of the song
Song::Song(juce::File _file) : file(_file), 
                                 title(_file.getFileNameWithoutExtension()),
                                 URL(juce::URL{ _file })
{
    DBG("Created new track with title: " << title);
}

bool Song::operator==(const juce::String& other) const 
{
    return title == other;
}