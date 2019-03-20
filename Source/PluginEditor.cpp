
#include "PluginProcessor.h"
#include "PluginEditor.h"

rmpAudioProcessorEditor::rmpAudioProcessorEditor(rmpAudioProcessor *p)
    : AudioProcessorEditor(p), processor (p), keyboardState(processor->getKBState())
{

    setSize (1175, 668);

    // Background Initialization
    Image bg = ImageCache::getFromMemory(BinaryData::background_png, BinaryData::background_pngSize);
    bgimage.setImage(bg);
    bgimage.toBack();
    addAndMakeVisible(bgimage);

    // Logo Initialization
    Image logo = ImageCache::getFromMemory(BinaryData::hyperia_png, BinaryData::hyperia_pngSize);
    logoimage.setBounds(25, 10, logo.getWidth(), logo.getHeight());
    logoimage.setImage(logo);
    logoimage.toFront(false);
    addAndMakeVisible(logoimage);

    // Grid Initialization
    Image grid = ImageCache::getFromMemory(BinaryData::grid_png, BinaryData::grid_pngSize);
    gridimage.setBounds(377, 220, grid.getWidth(), grid.getHeight());
    gridimage.setImage(grid);
    gridimage.toFront(false);
    addAndMakeVisible(gridimage);

    // Keyboard Initialization
    keyboardComponent.setKeyWidth(28);
    keyboardComponent.setScrollButtonsVisible(false);
    keyboardComponent.setAvailableRange(36, 127);
    addAndMakeVisible(keyboardComponent);

    // Library Initialization
    LibraryMenu.init(processor->libraryPath);
    LibraryMenu.setListener(this);
    addAndMakeVisible(LibraryMenu);

    // Main Panel Initializaton
    mainPanel.addRotarySlider(nullptr, Rectangle<int>(0, 0, 82, 82), "volume", Image());
    mainPanel.addRotarySlider(nullptr, Rectangle<int>(0, 155, 82, 82), "pan", Image());
    addAndMakeVisible(mainPanel);

    // Reverb Panel Initializaton
    adsrPanel.addRotarySlider(nullptr, Rectangle<int>(66, 18, 82, 82), "attack", Image());
    adsrPanel.addRotarySlider(nullptr, Rectangle<int>(191, 18, 82, 82), "decay", Image());
    adsrPanel.addRotarySlider(nullptr, Rectangle<int>(66, 170, 82, 82), "sustain", Image());
    adsrPanel.addRotarySlider(nullptr, Rectangle<int>(191, 170, 82, 82), "release", Image());
    addAndMakeVisible(adsrPanel);

/*

//  addAndMakeVisible(ReverbPanel );
    addAndMakeVisible(AdsrPanel);
    addAndMakeVisible(keyboardComponent);
    addAndMakeVisible(LibraryMenu);
    addAndMakeVisible(librarySlider);
    
    addAndMakeVisible(logoimage);
    addAndMakeVisible(gridimage);
    //addAndMakeVisible(layerPanel);
    addAndMakeVisible(funcPanel);

	Image grid = ImageCache::getFromMemory(BinaryData::grid_png, BinaryData::grid_pngSize);
	gridimage.setBounds(377, 220, grid.getWidth(), grid.getHeight());
	gridimage.setImage(grid);
	gridimage.toBack();

//	addAndMakeVisible(panSlider);
//	addAndMakeVisible(masterVolSlider);
	laf = std::make_shared<rmpLookAndFeel>();
	librarySlider.setLookAndFeel(laf.get());
	librarySlider.setValue(1);
	keyboardComponent.setKeyWidth(28);
	keyboardComponent.setScrollButtonsVisible(false);
	keyboardComponent.setAvailableRange(36,127);
	auto sliderLeft = 120;

	panSlider.setBounds(getWidth() - 250, 20, 200, 20);
	masterVolSlider.setBounds(getWidth() - 250, 50, 200, 20);


	AdsrPanel.setBounds(322, 233, 300, 317);
	AdsrPanel.setLookAndFeel(laf.get());

	funcPanel.setBounds(375, 60, 681, 131);
	funcPanel.setLookAndFeel(laf.get());

	ReverbPanel.setBounds(120, 300, 400, 150);
	LibraryMenu.setBounds(20, 50, 321, 480);
	

//	ReverbPanel.addRack( processor.getListener() );
//	AdsrPanel.addRack(processor.getListener());
//	funcPanel.addRack(processor.getListener());
//	funcPanel.setEffects(&AdsrPanel, &ReverbPanel);

	ReverbPanel.init();
	AdsrPanel.init();
	
	LibraryMenu.init(&processor);

	librarySlider.setName("library");
	librarySlider.setSliderStyle(Slider::LinearVertical);
	librarySlider.setTextBoxStyle(Slider::NoTextBox, true, 0, 0);
	librarySlider.addListener(&LibraryMenu);
	librarySlider.setRange(0, 1, 0);
	
	librarySlider.setBounds(355, 35, 22, 500);
  */
}

rmpAudioProcessorEditor::~rmpAudioProcessorEditor()
{
	librarySlider.setLookAndFeel(nullptr);
}

void rmpAudioProcessorEditor::instrumentSelected(String configName, XmlElement *config, SQLInputSource *source)
{
    processor->applyInstrumentConfig(configName, config, source);
    attachElements();
}

void rmpAudioProcessorEditor::attachElements()
{
    SummedLayersSound *sound = static_cast<SummedLayersSound *>(processor->getSynth()->getSound(0).get());
    
    // Link Main Panel
    mainPanel.setLink(sound->rack.findEffect("volume")->getLinkToParam("value"), "volume");
    mainPanel.setLink(sound->rack.findEffect("pan")->getLinkToParam("value"), "pan");

    // Link ADSR Panel
    adsrPanel.setLink(sound->rack.findEffect("adsr")->getLinkToParam("attack"), "attack");
    adsrPanel.setLink(sound->rack.findEffect("adsr")->getLinkToParam("decay"), "decay");
    adsrPanel.setLink(sound->rack.findEffect("adsr")->getLinkToParam("sustain"), "sustain");
    adsrPanel.setLink(sound->rack.findEffect("adsr")->getLinkToParam("release"), "release");
}

void rmpAudioProcessorEditor::paint (Graphics&)
{
	/* Image libraryback = ImageCache::getFromMemory(BinaryData::librarybackground_png, BinaryData::librarybackground_pngSize);
	LibraryMenu.getBounds();
	g.drawImage(libraryback, LibraryMenu.getBounds().getX()-1,
					  LibraryMenu.getBounds().getY()-1, 
					  LibraryMenu.getBounds().getWidth()+2, 
					  LibraryMenu.getBounds().getHeight()+2, 
		              0, 0, libraryback.getWidth(), libraryback.getHeight()); */
}

void rmpAudioProcessorEditor::resized()
{
     bgimage.setBounds(0, 0, 1175, 551);
     keyboardComponent.setBounds (0, getHeight() - 117, getWidth() , 117); 
     LibraryMenu.setBounds(20, 50, 321, 480);
     mainPanel.setBounds(1068, 251, 82, 241);
     adsrPanel.setBounds(322, 233, 300, 317);
}
