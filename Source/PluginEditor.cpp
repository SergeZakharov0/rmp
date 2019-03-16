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
    setSize (1200, 600);
    
    panSlider.setName("pan");
    panSlider.addListener( processor.getListener() );
    panSlider.setRange( -100, 100, 1);
    addAndMakeVisible(panSlider);
    panLabel.setText("Pan", dontSendNotification);
    panLabel.attachToComponent(&panSlider, true);
    
    masterVolSlider.setName("volume");
   masterVolSlider.addListener( processor.getListener() );
    masterVolSlider.setRange(0, 100, 1);
    addAndMakeVisible(masterVolSlider);
    masterVolSlider.setValue(100);
    masterVolLabel.setText("Volume", dontSendNotification);
    masterVolLabel.attachToComponent(&masterVolSlider, true);
    
	addAndMakeVisible( ReverbPanel );
	addAndMakeVisible(AdsrPanel);
	addAndMakeVisible(funcPanel);

	auto sliderLeft = 120;

	panSlider.setBounds(sliderLeft, 20, 200, 20);
	masterVolSlider.setBounds(sliderLeft, 50, 200, 20);


	AdsrPanel.setBounds(120, 100, 400, 150);
	ReverbPanel.setBounds(120, 300, 400, 150);
	funcPanel.setBounds(550, 300, 400, 150);

	ReverbPanel.addRack( processor.getListener() );
	AdsrPanel.addRack(processor.getListener());
	funcPanel.addRack(processor.getListener());
	funcPanel.setEffects(&AdsrPanel, &ReverbPanel);

	ReverbPanel.init();
	AdsrPanel.init();
	

	File libraryDir(processor.libraryPath);
	Array<File> childFiles = libraryDir.findChildFiles(2, false, String("*.rmp"));
	XmlElement *main = new XmlElement(String("main"));

	for (int step = 0; step < childFiles.size(); ++step) {
		SQLInputSource dbsource(String("desc.xml"), childFiles[step].getFullPathName());
		MemoryInputStream *stream = (MemoryInputStream *)dbsource.createInputStream();
		char *data = (char *)stream->getData();
		XmlElement *ex = new XmlElement(*parseXML(String(CharPointer_UTF8(data))));
		XmlElement *file_desc = new XmlElement(String("filedesc"));
		file_desc->setAttribute(String("path"), childFiles[step].getFullPathName());
		ex->addChildElement(file_desc);
		main->addChildElement(ex);
		delete stream;
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
    

    keyboardComponent.setBounds (0, getHeight() - 64, getWidth() , 64);
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
}
