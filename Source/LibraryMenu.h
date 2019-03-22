
#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "SQLInputSource.h"

struct intents_sizes {
	float packText;
	float folderWidth, folderHeight, folderIntent, folderText;
	float instrWidth, instrHeight, instrIntent, instrText;
	
	intents_sizes() {};
	void calculate(float imWidth, float imHeight) {
		packText = imWidth / 18; 

		folderHeight = 0.534f * imHeight;
		folderWidth = folderHeight * 1.262f;
		folderIntent = packText / 2;
		folderText = folderIntent + folderWidth + packText * 1.5f;

		instrHeight = 0.418f * imHeight;
		instrWidth = instrHeight * 1.87f;
		instrIntent = folderIntent + folderWidth;
		instrText = instrIntent + instrWidth + packText * 1.5f;
	}
	
};

class rmpDefaultMenuItem : public TreeViewItem {
	public:
		rmpDefaultMenuItem(String _name, bool _first = false, int _image_width = 343) {
			name = _name; 
			first = _first;
			image_width = _image_width; 
			image_height = (first) ? (int)(image_width / aspect_rate) : (int)(image_width / aspect_rate - vertical_intent);
			is.calculate((float)image_width, (float)image_height);
		};
		rmpDefaultMenuItem(rmpDefaultMenuItem &) = default;
		~rmpDefaultMenuItem() {
			clearSubItems();
		};
		int getItemWidth() const { return image_width; };
		int getItemHeight() const { return image_height; };
		virtual bool mightContainSubItems() { return true; };
		String getUniqueName() const { return name; };
		bool canBeSelected() const { return true; };
		Component *createItemComponent() { return nullptr; };
		void paintOpenCloseButton(Graphics &, const Rectangle< float >, Colour, bool) {};
		virtual void paintItem(Graphics &, int, int) {};
		virtual void itemClicked(const MouseEvent &) { this->setOpen(!( getOpenness() == opennessOpen) ); };
		String db_path;
	protected:
		String name;
		int image_width, image_height;
		float aspect_rate = 5.2f;
		float font_rate = 14.3f;
		bool first;
		const int vertical_intent = 3;
		intents_sizes is;
	};

class rmpPackMenuItem : public rmpDefaultMenuItem {
public:
	rmpPackMenuItem(String _name, bool _first = false, int _image_width = 343) : rmpDefaultMenuItem(_name, _first, _image_width){ };
	rmpPackMenuItem(rmpPackMenuItem &) = default;
	~rmpPackMenuItem() {
		clearSubItems();
	};
	void paintItem(Graphics &g, int width, int height);
};

class rmpInstrumentMenuItem : public rmpDefaultMenuItem {
public:
	rmpInstrumentMenuItem(String _name, bool _first = false, int _image_width = 343) : rmpDefaultMenuItem(_name, _first, _image_width) {};
	rmpInstrumentMenuItem(rmpInstrumentMenuItem &) = default;
	~rmpInstrumentMenuItem() {
		clearSubItems();
	};
	void paintItem(Graphics &g, int width, int height);
	void paintOpenCloseButton(Graphics &g, const Rectangle< float > &area, Colour backgroundColour, bool isMouseOver);
	void itemClicked(const MouseEvent &);
	String instr_path;
};

class rmpFolderMenuItem : public rmpDefaultMenuItem {
public:
	rmpFolderMenuItem(String _name, bool _first = false, int _image_width = 343) : rmpDefaultMenuItem(_name, _first, _image_width) {};
	rmpFolderMenuItem(rmpFolderMenuItem &) = default;
	~rmpFolderMenuItem() {
		clearSubItems();
	};
	void paintItem(Graphics &g, int width, int height);
	void paintOpenCloseButton(Graphics &g, const Rectangle< float > &area, Colour backgroundColour, bool isMouseOver);
};

class rmpLibraryMenu : public TreeView, public Slider::Listener {
	public:
		~rmpLibraryMenu() {
			deleteRootItem();
        };

        class Listener 
        {
        public:
            virtual void instrumentSelected(String, XmlElement*, SQLInputSource *) = 0;
        };

        void setListener(Listener *l)
        {
            listener = l;
        };

		void init(String libraryDirPath);
		void sliderValueChanged(Slider *slider);
	private:
        String selectedItemName;
        friend rmpInstrumentMenuItem;
        Listener *listener;
		void parseLevel(XmlElement *xml_node, rmpDefaultMenuItem *menu_node);
		XmlElement *parseLibdir(String libraryDirPath);


	};