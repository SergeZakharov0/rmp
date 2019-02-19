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
    
private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    NewProjectAudioProcessor& processor;
    
    MidiKeyboardState& keyboardState = processor.getKBState();
    MidiKeyboardComponent keyboardComponent  { keyboardState, MidiKeyboardComponent::horizontalKeyboard};
    
    Slider masterVolSlider;
    Slider panSlider;
    Label masterVolLabel;
    Label panLabel;

	class myListBoxModel : public TableListBoxModel {
	public:
		myListBoxModel() {};
		myListBoxModel(XmlElement *libraryXml, NewProjectAudioProcessor * _processor, TableListBox *_listbox) {
			data = libraryXml;
			processor = _processor;
			selected[0] = libraryXml;
			listbox = _listbox;
			for (int step = 1; step < 10; ++step)
				selected[step] = 0;

		};
			
		~myListBoxModel() { delete data; };
		int getNumRows() { 
			int m = 0;
			for (int step = 0; step < 10; step++)
				if (selected[step]) {
					int counter = 0;
					forEachXmlChildElementWithTagName(*selected[step], elem, String("folder"))
						++counter;
					forEachXmlChildElementWithTagName(*selected[step], elem, String("instrument"))
						++counter;
					forEachXmlChildElementWithTagName(*selected[step], elem, String("pack"))
						++counter;
					if (counter > m)
						m = counter;
					}
			return m; };
		void paintRowBackground(Graphics &g, int rowNumber, int width, int height, bool rowIsSelected) {
		};
		void paintCell(Graphics &g, int rowNumber, int columnid, int width, int height, bool rowIsSelected) { 
			if (selected[columnid - 1]) {
				XmlElement *now;
				for (now = selected[columnid - 1]->getFirstChildElement(); now != nullptr; now = now->getNextElement()) {
					if (!(now->getTagName() == String("instrument")) && !(now->getTagName() == String("folder")) && !(now->getTagName() == String("pack")))
						continue;
					if (!rowNumber)
						break;
					--rowNumber; }
				
				if (now)
					g.drawText(now->getChildByName(String("name"))->getAllSubText(), Rectangle<int>(width, height), Justification(36));
				
				};
		};
		void cellClicked(int rowNumber, int columnId, const MouseEvent & event) {
			XmlElement *now;
			int counter = 0;

			if (selected[columnId - 1] == nullptr)
				return;

			for (now = selected[columnId - 1]->getFirstChildElement(); now != nullptr; now = now->getNextElement()) 
				if ((now->getTagName() == String("instrument")) || (now->getTagName() == String("folder")) || (now->getTagName() == String("pack"))) {
					counter++;
					if (counter > rowNumber)
						break;
				}
			if (now == nullptr)
				return;
			if (now->getTagName() == String("pack") || now->getTagName() == String("folder"))
				selected[columnId] = now;
			if (now->getTagName() == String("instrument")) {
				String abs_path = now->getChildByName(String("file"))->getAllSubText();
				while (!now->hasTagName(String("pack")))
					now = data->findParentElementOf(now);
				abs_path = processor->libraryPath + String("\\") + now->getChildByName(String("path"))->getAllSubText() + String("\\") + abs_path;
				processor->applyInstrumentConfig(abs_path);
				}
			listbox->updateContent();
			listbox->repaint();
		};

	private:
		XmlElement *data; 
		XmlElement *selected[10];
		NewProjectAudioProcessor * processor;
		TableListBox *listbox;
	};

	TableListBox listBox;
	std::shared_ptr<myListBoxModel> listModel;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (NewProjectAudioProcessorEditor)
};
