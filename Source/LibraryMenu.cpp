
#include "LibraryMenu.h"

void rmpPackMenuItem::paintItem(Graphics &g, int width, int height) {
 	Image im = ImageCache::getFromMemory(BinaryData::libraryback_pack_png, BinaryData::libraryback_pack_pngSize);
	g.drawImage(im, 0, 0, width, height, 0, (!first) ? vertical_intent : 0, 484, 93 - ((!first) ? vertical_intent : 0));
	g.setFont(Font(String("Sans"), String("Serif"), (int)image_width / font_rate));
	g.drawText(name, is.packText, 0, width - is.packText, height, Justification(1));
}

void rmpInstrumentMenuItem::paintItem(Graphics &g, int width, int height) {
	Image im = ImageCache::getFromMemory(BinaryData::libraryback_greyfull_png, BinaryData::libraryback_greyfull_pngSize);
	g.drawImage(im, 0, 0, width, height, 0, vertical_intent, 484, 93 - vertical_intent);
	g.setFont(Font(String("Sans"), String("Serif"), (int)image_width / font_rate));
	g.drawText(name, is.instrText, 0, width - is.instrText, height, Justification(1));
}

void rmpInstrumentMenuItem::paintOpenCloseButton(Graphics &g, const Rectangle< float > &area, Colour backgroundColour, bool isMouseOver) {
	Image im = ImageCache::getFromMemory(BinaryData::instrument_png, BinaryData::instrument_pngSize);
	Rectangle<float> _area(area);
	_area.setWidth(is.instrWidth);
	_area.setHeight(is.instrHeight);
	_area.setX(is.instrIntent);
	_area.setY((image_height - is.instrHeight) / 2);
	g.drawImage(im, _area, 128);
}

void rmpFolderMenuItem::paintItem(Graphics &g, int width, int height) {
	Image im = ImageCache::getFromMemory(BinaryData::libraryback_greyfull_png, BinaryData::libraryback_greyfull_pngSize);
	g.drawImage(im, 0, 0, width, height, 0, vertical_intent, 484, 93 - vertical_intent);
	g.setFont(Font(String("Sans"), String("Serif"), (int)image_width / font_rate));
	g.drawText(name, is.folderText, 0, width - is.folderText, height, Justification(1));
}

void rmpFolderMenuItem::paintOpenCloseButton(Graphics &g, const Rectangle< float > &area, Colour backgroundColour, bool isMouseOver) {
	Image im;
	if (this->isOpen())
		im = ImageCache::getFromMemory(BinaryData::folder_active_png, BinaryData::folder_active_pngSize);
	else
		im = ImageCache::getFromMemory(BinaryData::folder_inactive_png, BinaryData::folder_inactive_pngSize);
	Rectangle<float> _area(area);
	_area.setWidth(is.folderWidth);
	_area.setHeight(is.folderHeight);
	_area.setX(is.folderIntent);
	_area.setY((image_height - is.folderHeight) / 2);
	g.drawImage(im, _area, 128);
}

void rmpLibraryMenu::parseLevel(XmlElement *xml_node, rmpDefaultMenuItem *menu_node) {
	XmlElement *xml_child;
	forEachXmlChildElement(*xml_node, xml_child) {
		if  (xml_child->getTagName() == String("instrument")) {
			rmpInstrumentMenuItem *menu_child = new rmpInstrumentMenuItem(String(xml_child->getChildByName(String("name"))->getAllSubText()), false, getBounds().getWidth());
			menu_child->db_path = menu_node->db_path;
			menu_child->instr_path = xml_child->getChildByName(String("file"))->getAllSubText();
			menu_node->addSubItem(menu_child);
			parseLevel(xml_child, menu_child);
			}
		if (xml_child->getTagName() == String("pack")) {
			static bool first = true;
			rmpDefaultMenuItem *menu_child = new rmpPackMenuItem(String(xml_child->getChildByName(String("name"))->getAllSubText()), first, getBounds().getWidth());
			menu_child->db_path = xml_child->getChildByName(String("filedesc"))->getAttributeValue(0);
			first = false;
			menu_node->addSubItem(menu_child);
			parseLevel(xml_child, menu_child);
		}
		if (xml_child->getTagName() == String("folder")) {
			rmpDefaultMenuItem *menu_child = new rmpFolderMenuItem(String(xml_child->getChildByName(String("name"))->getAllSubText()), false, getBounds().getWidth());
			menu_child->db_path = menu_node->db_path;
			menu_node->addSubItem(menu_child);
			parseLevel(xml_child, menu_child);
		}
	}
}

void rmpLibraryMenu::init(String libraryDirPath) {
	setIndentSize(0);
	this->getViewport()->setScrollBarsShown(false, false, false, false);
	XmlElement *xml_child;
	rmpDefaultMenuItem *root_item = new rmpDefaultMenuItem(String("main"), false, getBounds().getWidth());
	this->setRootItem(root_item);
	this->setRootItemVisible(false);
	root_item->setOpen(true);
	XmlElement *main = parseLibdir(libraryDirPath);
	parseLevel(main, root_item);
	delete(main);
}

XmlElement *rmpLibraryMenu::parseLibdir(String libraryDirPath) {
	File libraryDir(libraryDirPath);
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

	return main;
}

void rmpLibraryMenu::sliderValueChanged(Slider *slider) {
	this->getViewport()->setViewPositionProportionately(0, 1 - slider->getValue());
	}

void rmpInstrumentMenuItem::itemClicked(const MouseEvent &) {
    rmpLibraryMenu *menu = static_cast<rmpLibraryMenu *>(this->getOwnerView());
    if (menu->selectedItemName == name)
    {

    }
    else 
    {
        menu->selectedItemName = name;
        SQLInputSource dbsource(instr_path, db_path);
        MemoryInputStream *stream = (MemoryInputStream *)dbsource.createInputStream();
        char *data = (char *)stream->getData();
        XmlElement *ex = new XmlElement(*parseXML(String(CharPointer_UTF8(data))));
        static_cast<rmpLibraryMenu *>(this->getOwnerView())->listener->instrumentSelected(name, ex, &dbsource);
        delete ex;
        delete stream;
    }

};

