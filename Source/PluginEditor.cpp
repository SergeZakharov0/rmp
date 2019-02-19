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
    
	File libraryDir(processor.libraryPath);
	Array<File> childFiles = libraryDir.findChildFiles(2, false, String("*.xml"));
	XmlElement *main = new XmlElement(String("main"));

	for (int step = 0; step < childFiles.size(); ++step) {
		XmlElement *ex = new XmlElement(*parseXML(childFiles[step]).get());
		main->addChildElement(ex);
		}

	listModel = std::make_shared<myListBoxModel>(main, &processor, &listBox);
	listBox.setModel(listModel.get());
	listBox.setBounds(Rectangle<int>(500, 20, 400, 200));
	listBox.getHeader().addColumn(String("one"), 1, 100);
	listBox.getHeader().addColumn(String("two"), 2, 100);
	listBox.getHeader().addColumn(String("three"), 3, 100);
	listBox.getHeader().addColumn(String("four"), 4, 100);
	addAndMakeVisible(listBox);
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
