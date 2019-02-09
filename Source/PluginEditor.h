/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"
#include <stdlib.h>

//==============================================================================
/**
*/
class NewProjectAudioProcessorEditor  : public AudioProcessorEditor
{
public:
    NewProjectAudioProcessorEditor (NewProjectAudioProcessor&);
    ~NewProjectAudioProcessorEditor();

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;
    //==============================================================================
    
    char message[128];
private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    NewProjectAudioProcessor& processor;
    
    MidiKeyboardState& keyboardState = processor.getKBState();
    MidiKeyboardComponent keyboardComponent  { keyboardState, MidiKeyboardComponent::horizontalKeyboard};
    
    Slider& masterVolSlider = processor.getMVSlider();
    Slider& panSlider = processor.getPSlider();
    Label masterVolLabel;
    Label panLabel;
    
 
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NewProjectAudioProcessorEditor)
};
