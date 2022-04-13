
#include <JuceHeader.h>
#include "Graph.h"
#include <iomanip>
#include <sstream>

//==============================================================================
graphDisplay::graphDisplay()
{
    // initialise any special settings that your component needs.
    setGridLineCount();
    setRange(); //sets to default
    initCoords(75.0f, 75.0f);
}

graphDisplay::~graphDisplay() {}
graphDisplay::Listener::Listener() {}
graphDisplay::Listener::~Listener() {}


void graphDisplay::paint (juce::Graphics& g)
{
    g.fillAll (getLookAndFeel().findColour (juce::ResizableWindow::backgroundColourId));   // clear the background
    g.setColour (juce::Colours::black);
    drawGraph(g);
    g.setColour(juce::Colours::red);
    drawPointer(g);
    g.setColour(juce::Colours::white);
    if (pointerMoved) { drawText(g); }

    // capture raw range for reference when resizing
    setRangeInitial();
}

void graphDisplay::resized()
{
    setSettings();
    updateCoords();
}

// activate the pointer when mouse is pressed down on the graph
void graphDisplay::mouseDown(const juce::MouseEvent& event)
{
    // remove mouse cursor to make users feel like they are only moving pointer
    DBG("Mouse Clicked on plot at: " << getX() << "," << getY());
    pointerMoved = true;
    setMouseCursor(juce::MouseCursor::NoCursor);

    // update the coords 
    setCoords(float(event.getMouseDownX()), float(event.getMouseDownY()));
    interactComponent();
    repaint();
}

// move the pointer corresponding to the mouse position
void graphDisplay::mouseDrag(const juce::MouseEvent& event)
{
    DBG("Mouse dragged to: " << getX() << ", " << getY());
    juce::Point<int> rawPos(event.getPosition());
    float initialX = float(rawPos.getX());
    float initialY = float(rawPos.getY());

    // record the position 
    setCoords(initialX, initialY);
    interactComponent();
    repaint();
}

// return the cursor once mouse button is released
void graphDisplay::mouseUp(const juce::MouseEvent& event)
{
    setMouseCursor(juce::MouseCursor::NormalCursor);
}

// allow interactions with other Components
void graphDisplay::interactComponent()
{
    listeners.call([this](Listener& l) { l.graphValueChange(this); });
}
void graphDisplay::addListener(Listener* l) 
{ 
    listeners.add(l); 
}
void graphDisplay::removeListener(Listener* l) 
{ 
    listeners.remove(l); 
}

// Configuration for the graph
float graphDisplay::getX() 
{
    return constrain(coordsInitial['x']); 
}

float graphDisplay::getY()
{
    return invertCoord(constrain(coordsInitial['y']), range['min'], range['max']);
}

void graphDisplay::setGridLineCount(int lineCount)
{
    if (lineCount % 2 == 1){--lineCount;}
    gridLineCount = lineCount;
}

void graphDisplay::setRange(float min, float max)
{
    range['min'] = min;
    range['max'] = max;
}

void graphDisplay::initCoords(float initialX, float initialY)
{
    coordsInitial['x'] = initialX;
    coordsInitial['y'] = initialY;
}

// change the coords
void graphDisplay::setCoords(float initialX, float initialY)
{
    if(inRangeInitial(initialX, initialY)) { coordsInitial['x'] = initialX, coordsInitial['y'] = initialY; }
}

// update the values as the point is moved
void graphDisplay::updateCoords()
{
    //get ratios based off initial range
    double xRatio = double(coordsInitial['x'] / (rangeInitial['max'] - rangeInitial['min']));
    double yRatio = double(coordsInitial['y'] / (rangeInitial['max'] - rangeInitial['min']));

    // new x and y based off current size and previous ratio
    float newX = float(right * xRatio);
    float newY = float(bottom * yRatio);

    setCoords(newX, newY);
}

// 
void graphDisplay::setRangeInitial()
{
    rangeInitial['min'] = getLocalBounds().getX();
    rangeInitial['max'] = getLocalBounds().getWidth();
}

void graphDisplay::drawGraph(juce::Graphics& g)
{
    g.drawRect(getLocalBounds(), 3);// draw an outline around the component
    drawAxis(g);
    drawGrid(g);
}

void graphDisplay::drawAxis(juce::Graphics& g)
{
    //draw x and y axis
    g.setColour(juce::Colours::black);
    g.drawLine(left, midY, right, midY, 2);
    g.drawLine(midX, left, midX, bottom, 2);
}


void graphDisplay::drawGrid(juce::Graphics& g)
{
    const float myDashLength[] = { 3, 3 };
    float offset = float(getLocalBounds().getWidth() / (gridLineCount + 2));

    for (int i = 0; i < (gridLineCount/2); ++i)
    {
        int d{ i + 1 }; //degrees away from axis
        //draw to left/right of Y-axis and top/bottom of X-axis
        g.setColour(juce::Colours::black);
        g.drawDashedLine(juce::Line<float>(midX - offset * d, top, midX - offset * d, bottom),
            &myDashLength[0], 2, 1.0, 0);
        g.drawDashedLine(juce::Line<float>(midX + offset * d, top, midX + offset * d, bottom),
            &myDashLength[0], 2, 1.0, 0);
        g.drawDashedLine(juce::Line<float>(left, midY - offset * d, right, midY - offset * d),
            &myDashLength[0], 2, 1.0, 0);
        g.drawDashedLine(juce::Line<float>(left, midY + offset * d, right, midY + offset * d),
            &myDashLength[0], 2, 1.0, 0);
    }
}

void graphDisplay::drawPointer(juce::Graphics& g)
{
    //set length of cursor
    float length = float(getLocalBounds().getWidth() / 15);

    //create lines
    juce::Line<float> horizontalL(juce::Point<float>(coordsInitial['x'] - length, coordsInitial['y']),
        juce::Point<float>(coordsInitial['x'] + length, coordsInitial['y']));
    juce::Line<float> verticalL(juce::Point<float>(coordsInitial['x'], coordsInitial['y'] - length),
        juce::Point<float>(coordsInitial['x'], coordsInitial['y'] + length));
    
    //draw lines
    g.drawLine(horizontalL, 2.0f);
    g.drawLine(verticalL, 2.0f);
}


void graphDisplay::drawText(juce::Graphics& g)
{
    // decorate text
    g.setFont(float(getWidth()/12));
    int textHeight = int(g.getCurrentFont().getHeight());

    //draw Y value
    std::stringstream streamY;
    streamY << std::fixed << std::setprecision(2) << getY();
    g.drawText(streamY.str(), int(midX), int(top) + 5, int(midX), textHeight, 
        juce::Justification::centredLeft, true);

    //draw X value
    std::stringstream streamX;
    streamX << std::fixed << std::setprecision(2) << getX();
    g.drawText(streamX.str(), int(midX) - 5, int(midY), int(midX), textHeight, 
        juce::Justification::centredRight, true);
}

void graphDisplay::setSettings()
{
    //recalculate all the settings
    midY = float(getLocalBounds().getCentreY());
    midX = float(getLocalBounds().getCentreX());
    left = float(getLocalBounds().getX());
    right = float(getLocalBounds().getRight());
    top = float(getLocalBounds().getY());
    bottom = float(getLocalBounds().getBottom());
}

// limit the values of the graph
float graphDisplay::constrain(float coord)
{
    float oldRangeMin = float(getLocalBounds().getX());
    float oldRangeMax = float(getLocalBounds().getWidth());
    float oldRange = oldRangeMax - oldRangeMin;
    float newRange = range['max'] - range['min'];

    float newValue = (((coord - oldRangeMin) * newRange) / oldRange) + range['min'];
    return newValue;
}

/**Inverts coordinate within a range between min and max*/
float graphDisplay::invertCoord(float coord, float min, float max)
{
    return (min + max) - coord;
}
bool graphDisplay::inRangeInitial(float initialX, float initialY)
{
    return (initialX >= left && initialX <= right && initialY >= top && initialY <= bottom);
}