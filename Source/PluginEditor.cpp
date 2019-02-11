/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
NewProjectAudioProcessorEditor::NewProjectAudioProcessorEditor (NewProjectAudioProcessor& p)
    : AudioProcessorEditor (&p), processor (p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (1200, 300);
    strcpy(message, "I am initial");
    
    panSlider.setName("pan");
    panSlider.addListener( static_cast<Slider::Listener*>(&processor) );
    panSlider.setRange( -100, 100, 1);
    addAndMakeVisible(panSlider);
    panLabel.setText("Pan", dontSendNotification);
    panLabel.attachToComponent(&panSlider, true);
    
    masterVolSlider.setName("volume");
    masterVolSlider.addListener( static_cast<Slider::Listener*>(&processor) );
    masterVolSlider.setRange(0, 100, 1);
    addAndMakeVisible(masterVolSlider);
    masterVolSlider.setValue(100);
    masterVolLabel.setText("Volume", dontSendNotification);
    masterVolLabel.attachToComponent(&masterVolSlider, true);
    
    addAndMakeVisible (keyboardComponent);
}

NewProjectAudioProcessorEditor::~NewProjectAudioProcessorEditor()
{
}

//==============================================================================
void NewProjectAudioProcessorEditor::paint (Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
   
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));

    g.setColour (Colours::white);
    g.setFont (15.0f);
    g.drawFittedText (message, getLocalBounds(), Justification::centred, 1);
    
}

void NewProjectAudioProcessorEditor::resized()
{
    auto sliderLeft = 120;
    
    panSlider .setBounds (sliderLeft, 20, 200, 20);
    masterVolSlider.setBounds (sliderLeft, 50, 200, 20);
    
    keyboardComponent.setBounds (0, 236, getWidth() , 64);
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
}
