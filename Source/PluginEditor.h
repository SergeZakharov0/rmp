#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "RackControlPanel.h"
#include <stdlib.h>
#include "LibraryMenu.h"
#include "PluginProcessor.h"

class rmpAudioProcessor;

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
    
	ImageComponent bgimage;
	ImageComponent logoimage;
	ImageComponent gridimage;

    MidiKeyboardState& keyboardState;
    MidiKeyboardComponent keyboardComponent  { keyboardState, MidiKeyboardComponent::horizontalKeyboard};
    
    Slider masterVolSlider;
	Slider librarySlider;
    Slider panSlider;
    Label masterVolLabel;
    Label panLabel;

    EffectControlPanel mainPanel, layerPanel, reverbPanel, adsrPanel, funcPanel;
    rmpLibraryMenu     LibraryMenu;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (rmpAudioProcessorEditor)
};
