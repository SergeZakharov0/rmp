#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "RackControlPanel.h"
#include <stdlib.h>
#include "LibraryMenu.h"
#include "PluginProcessor.h"

class rmpAudioProcessor;

class rmpKeyboard : public MidiKeyboardComponent
{
public:
    rmpKeyboard() = default;
    rmpKeyboard(MidiKeyboardState &state, MidiKeyboardComponent::Orientation orient) : MidiKeyboardComponent(state, orient) 
    {
        clearKeyMappings();
        setColour(ColourIds::mouseOverKeyOverlayColourId, Colour((uint8)150, (uint8)150, (uint8)150, (uint8)128));
        setColour(ColourIds::keyDownOverlayColourId, Colour((uint8)90, (uint8)90, (uint8)90, (uint8)128));
    };
};

class rmpAudioProcessorEditor  : public AudioProcessorEditor, public rmpLibraryMenu::Listener
{
public:
    rmpAudioProcessorEditor(rmpAudioProcessor *);
    ~rmpAudioProcessorEditor();


    void instrumentSelected(String configName, XmlElement *config, SQLInputSource *source) override;
    void attachElements();

    void paint (Graphics&) override;
    void resized() override;
    
private:
    rmpAudioProcessor *processor;
    Rectangle<int> prev_bounds;

	ImageComponent bgimage, logoimage, gridimage;
    ImageComponent volumetext, pantext, envelopetext, attacktext, sustaintext, releasetext, decaytext, reverbtext, widthtext,
        dampingtext, reverbheadtext, delaytext, delayheadtext, feedbacktext, timetext, libraryback;

    MidiKeyboardState& keyboardState;
    rmpKeyboard keyboardComponent  { keyboardState, MidiKeyboardComponent::horizontalKeyboard};
    
    Slider masterVolSlider;
	Slider librarySlider;
    Slider panSlider;
    Label masterVolLabel;
    Label panLabel;

    EffectControlPanel mainPanel, layerPanel, reverbdelayPanel, adsrPanel, funcPanel;
    rmpLibraryMenu     LibraryMenu;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (rmpAudioProcessorEditor)
};
