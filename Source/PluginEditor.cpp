
#include "PluginProcessor.h"
#include "PluginEditor.h"

rmpAudioProcessorEditor::rmpAudioProcessorEditor(rmpAudioProcessor *p)
    : AudioProcessorEditor(p), processor (p), keyboardState(processor->getKBState())
{

    const float resizeCoeff = 0.446;
    setSize(2242 * resizeCoeff, 1346 * resizeCoeff);
    // Background Initialization
    Image bg = ImageCache::getFromMemory(BinaryData::background_png, BinaryData::background_pngSize);
    bgimage.setBounds(0, 0, bg.getWidth() * resizeCoeff, bg.getHeight() * resizeCoeff);
    bgimage.setImage(bg, RectanglePlacement::Flags::stretchToFit);
    bgimage.toBack();
    addAndMakeVisible(bgimage);

    // Logo Initialization
    Image logo = ImageCache::getFromMemory(BinaryData::hyperia_png, BinaryData::hyperia_pngSize);
    logoimage.setBounds(24 * resizeCoeff, 8 * resizeCoeff, logo.getWidth() * resizeCoeff, logo.getHeight() * resizeCoeff);
    logoimage.setImage(logo, RectanglePlacement::Flags::stretchToFit);
    logoimage.toFront(false);
    addAndMakeVisible(logoimage);

    // Grid Initialization
    Image grid = ImageCache::getFromMemory(BinaryData::grid_png, BinaryData::grid_pngSize);
    gridimage.setBounds(691 * resizeCoeff, 445 * resizeCoeff, grid.getWidth() * resizeCoeff, grid.getHeight() * resizeCoeff);
    gridimage.setImage(grid, RectanglePlacement::Flags::stretchToFit);
    gridimage.toFront(false);
    addAndMakeVisible(gridimage);
 
    // Keyboard Initialization
    keyboardComponent.setKeyWidth(52 * resizeCoeff);
    keyboardComponent.setScrollButtonsVisible(false);
    keyboardComponent.setAvailableRange(36, 127);
    keyboardComponent.setBounds(2 * resizeCoeff, 1071 * resizeCoeff, 2242 * resizeCoeff, 275 * resizeCoeff);
    addAndMakeVisible(keyboardComponent);

    // Library Initialization
    Image librarybackimage = ImageCache::getFromMemory(BinaryData::libraryback_png, BinaryData::libraryback_pngSize);
    libraryback.setBounds(20 * resizeCoeff, 113 * resizeCoeff, 593 * resizeCoeff, 920 * resizeCoeff);
    libraryback.setImage(librarybackimage, RectanglePlacement::Flags::fillDestination);
    libraryback.toBack();
    addAndMakeVisible(libraryback);

    LibraryMenu.setBounds(20 * resizeCoeff, 113 * resizeCoeff, 593 * resizeCoeff, 920 * resizeCoeff);
    LibraryMenu.init(processor->libraryPath);
    LibraryMenu.setListener(this);
    addAndMakeVisible(LibraryMenu);

    Image rotarybg = ImageCache::getFromMemory(BinaryData::rotarybackground_png, BinaryData::rotarybackground_pngSize);
    Image buttonactiveimage = ImageCache::getFromMemory(BinaryData::buttonactive_png, BinaryData::buttonactive_pngSize);
    // Main Panel Initializaton
    Image volumetextimage = ImageCache::getFromMemory(BinaryData::volumetext_png, BinaryData::volumetext_pngSize);
    volumetext.setBounds(2052 * resizeCoeff, 691 * resizeCoeff, volumetextimage.getWidth() * resizeCoeff, volumetextimage.getHeight() * resizeCoeff);
    volumetext.setImage(volumetextimage, RectanglePlacement::Flags::stretchToFit);
    volumetext.toFront(false);
    addAndMakeVisible(volumetext);
    Image pantextimage = ImageCache::getFromMemory(BinaryData::pantext_png, BinaryData::pantext_pngSize);
    pantext.setBounds(2085 * resizeCoeff, 988 * resizeCoeff, pantextimage.getWidth() * resizeCoeff, pantextimage.getHeight() * resizeCoeff);
    pantext.setImage(pantextimage, RectanglePlacement::Flags::stretchToFit);
    pantext.toFront(false);
    addAndMakeVisible(pantext);
    mainPanel.setBounds(2052 * resizeCoeff, 517 * resizeCoeff, 159 * resizeCoeff, 503 * resizeCoeff);
    mainPanel.addRotarySlider(nullptr, "",  Rectangle<int>(2 * resizeCoeff, 0 * resizeCoeff, rotarybg.getWidth() * resizeCoeff, rotarybg.getHeight() * resizeCoeff), "volume", Image());
    mainPanel.addRotarySlider(nullptr, "",  Rectangle<int>(2 * resizeCoeff, 289 * resizeCoeff, rotarybg.getWidth() * resizeCoeff, rotarybg.getHeight() * resizeCoeff), "pan", Image());
    addAndMakeVisible(mainPanel);

    // ADSR Panel Initializaton
    Image envelopetextimage = ImageCache::getFromMemory(BinaryData::envelopetext_png, BinaryData::envelopetext_pngSize);
    envelopetext.setBounds(840 * resizeCoeff, 423 * resizeCoeff, envelopetextimage.getWidth() * resizeCoeff, envelopetextimage.getHeight() * resizeCoeff);
    envelopetext.setImage(envelopetextimage, RectanglePlacement::Flags::stretchToFit);
    envelopetext.toFront(false);
    addAndMakeVisible(envelopetext);
    Image attacktextimage = ImageCache::getFromMemory(BinaryData::attacktext_png, BinaryData::attacktext_pngSize);
    attacktext.setBounds(726 * resizeCoeff, 692 * resizeCoeff, attacktextimage.getWidth() * resizeCoeff, attacktextimage.getHeight() * resizeCoeff);
    attacktext.setImage(attacktextimage, RectanglePlacement::Flags::stretchToFit);
    attacktext.toFront(false);
    addAndMakeVisible(attacktext);
    Image sustaintextimage = ImageCache::getFromMemory(BinaryData::sustaintext_png, BinaryData::sustaintext_pngSize);
    sustaintext.setBounds(719 * resizeCoeff, 979 * resizeCoeff, sustaintextimage.getWidth() * resizeCoeff, sustaintextimage.getHeight() * resizeCoeff);
    sustaintext.setImage(sustaintextimage, RectanglePlacement::Flags::stretchToFit);
    sustaintext.toFront(false);
    addAndMakeVisible(sustaintext);
    Image decaytextimage = ImageCache::getFromMemory(BinaryData::decaytext_png, BinaryData::decaytext_pngSize);
    decaytext.setBounds(964 * resizeCoeff, 692 * resizeCoeff, decaytextimage.getWidth() * resizeCoeff, decaytextimage.getHeight() * resizeCoeff);
    decaytext.setImage(decaytextimage, RectanglePlacement::Flags::stretchToFit);
    decaytext.toFront(false);
    addAndMakeVisible(decaytext);
    Image releasetextimage = ImageCache::getFromMemory(BinaryData::releasetext_png, BinaryData::releasetext_pngSize);
    releasetext.setBounds(953 * resizeCoeff, 977 * resizeCoeff, releasetextimage.getWidth() * resizeCoeff, releasetextimage.getHeight() * resizeCoeff);
    releasetext.setImage(releasetextimage, RectanglePlacement::Flags::stretchToFit);
    releasetext.toFront(false);
    addAndMakeVisible(releasetext);
    adsrPanel.setBounds(717 * resizeCoeff, 417 * resizeCoeff, 397 * resizeCoeff, 594 * resizeCoeff);
    adsrPanel.addRotarySlider(nullptr, "", Rectangle<int>(0 * resizeCoeff, 100 * resizeCoeff, rotarybg.getWidth() * resizeCoeff, rotarybg.getHeight() * resizeCoeff), "attack", Image());
    adsrPanel.addRotarySlider(nullptr, "", Rectangle<int>(238 * resizeCoeff, 100 * resizeCoeff, rotarybg.getWidth() * resizeCoeff, rotarybg.getHeight() * resizeCoeff), "decay", Image());
    adsrPanel.addRotarySlider(nullptr, "", Rectangle<int>(3 * resizeCoeff, 389 * resizeCoeff, rotarybg.getWidth() * resizeCoeff, rotarybg.getHeight() * resizeCoeff), "sustain", Image());
    adsrPanel.addRotarySlider(nullptr, "", Rectangle<int>(243 * resizeCoeff, 389 * resizeCoeff, rotarybg.getWidth() * resizeCoeff, rotarybg.getHeight() * resizeCoeff), "release", Image());
    adsrPanel.addButton(nullptr, "", Rectangle<int>(68 * resizeCoeff, 0 * resizeCoeff, buttonactiveimage.getWidth() * resizeCoeff, buttonactiveimage.getHeight() * resizeCoeff), "onOff", Image());
    addAndMakeVisible(adsrPanel);

    // Reverb Panel Initializaton
    Image reverbtextimage = ImageCache::getFromMemory(BinaryData::reverbtext_png, BinaryData::reverbtext_pngSize);
    reverbtext.setBounds(1408 * resizeCoeff, 425 * resizeCoeff, reverbtextimage.getWidth() * resizeCoeff, reverbtextimage.getHeight() * resizeCoeff);
    reverbtext.setImage(reverbtextimage, RectanglePlacement::Flags::stretchToFit);
    reverbtext.toFront(false);
    addAndMakeVisible(reverbtext);
    Image widthtextimage = ImageCache::getFromMemory(BinaryData::widthtext_png, BinaryData::widthtext_pngSize);
    widthtext.setBounds(1245 * resizeCoeff, 691 * resizeCoeff, widthtextimage.getWidth() * resizeCoeff, widthtextimage.getHeight() * resizeCoeff);
    widthtext.setImage(widthtextimage, RectanglePlacement::Flags::stretchToFit);
    widthtext.toFront(false);
    addAndMakeVisible(widthtext);
    Image dampingtextimage = ImageCache::getFromMemory(BinaryData::dampingtext_png, BinaryData::dampingtext_pngSize);
    dampingtext.setBounds(1493 * resizeCoeff, 691 * resizeCoeff, dampingtextimage.getWidth() * resizeCoeff, dampingtextimage.getHeight() * resizeCoeff);
    dampingtext.setImage(dampingtextimage, RectanglePlacement::Flags::stretchToFit);
    dampingtext.toFront(false);
    addAndMakeVisible(dampingtext);
    Image reverbheadtextimage = ImageCache::getFromMemory(BinaryData::reverbheadtext_png, BinaryData::reverbheadtext_pngSize);
    reverbheadtext.setBounds(1219 * resizeCoeff, 978 * resizeCoeff, reverbheadtextimage.getWidth() * resizeCoeff, reverbheadtextimage.getHeight() * resizeCoeff);
    reverbheadtext.setImage(reverbheadtextimage, RectanglePlacement::Flags::stretchToFit);
    reverbheadtext.toFront(false);
    addAndMakeVisible(reverbheadtext);
    reverbdelayPanel.addRotarySlider(nullptr, "", Rectangle<int>(16 * resizeCoeff, 383 * resizeCoeff, rotarybg.getWidth() * resizeCoeff, rotarybg.getHeight() * resizeCoeff), "reverbDryWet", Image());
    reverbdelayPanel.addRotarySlider(nullptr, "", Rectangle<int>(295 * resizeCoeff, 95 * resizeCoeff, rotarybg.getWidth() * resizeCoeff, rotarybg.getHeight() * resizeCoeff), "roomSize", Image());
    reverbdelayPanel.addRotarySlider(nullptr, "", Rectangle<int>(17 * resizeCoeff, 95 * resizeCoeff, rotarybg.getWidth() * resizeCoeff, rotarybg.getHeight() * resizeCoeff), "width", Image());
    reverbdelayPanel.addButton(nullptr, "", Rectangle<int>(133 * resizeCoeff, -4 * resizeCoeff, buttonactiveimage.getWidth() * resizeCoeff, buttonactiveimage.getHeight() * resizeCoeff), "reverbOnOff", Image());

    // Delay Panel Initializaton
    Image delaytextimage = ImageCache::getFromMemory(BinaryData::delaytext_png, BinaryData::delaytext_pngSize);
    delaytext.setBounds(1824 * resizeCoeff, 423 * resizeCoeff, delaytextimage.getWidth() * resizeCoeff, delaytextimage.getHeight() * resizeCoeff);
    delaytext.setImage(delaytextimage, RectanglePlacement::Flags::stretchToFit);
    delaytext.toFront(false);
    addAndMakeVisible(delaytext);
    Image delayheadtextimage = ImageCache::getFromMemory(BinaryData::delayheadtext_png, BinaryData::delayheadtext_pngSize);
    delayheadtext.setBounds(1785 * resizeCoeff, 691 * resizeCoeff, delayheadtextimage.getWidth() * resizeCoeff, delayheadtextimage.getHeight() * resizeCoeff);
    delayheadtext.setImage(delayheadtextimage, RectanglePlacement::Flags::stretchToFit);
    delayheadtext.toFront(false);
    addAndMakeVisible(delayheadtext);
    Image feedbacktextimage = ImageCache::getFromMemory(BinaryData::feedbacktext_png, BinaryData::feedbacktext_pngSize);
    feedbacktext.setBounds(1486 * resizeCoeff, 976 * resizeCoeff, feedbacktextimage.getWidth() * resizeCoeff, feedbacktextimage.getHeight() * resizeCoeff);
    feedbacktext.setImage(feedbacktextimage, RectanglePlacement::Flags::stretchToFit);
    feedbacktext.toFront(false);
    addAndMakeVisible(feedbacktext);
    Image timetextimage = ImageCache::getFromMemory(BinaryData::timetext_png, BinaryData::timetext_pngSize);
    timetext.setBounds(1824 * resizeCoeff, 978 * resizeCoeff, timetextimage.getWidth() * resizeCoeff, timetextimage.getHeight() * resizeCoeff);
    timetext.setImage(timetextimage, RectanglePlacement::Flags::stretchToFit);
    timetext.toFront(false);
    addAndMakeVisible(timetext);
    reverbdelayPanel.addRotarySlider(nullptr, "", Rectangle<int>(579 * resizeCoeff, 94 * resizeCoeff, rotarybg.getWidth() * resizeCoeff, rotarybg.getHeight() * resizeCoeff), "delayDryWet", Image());
    reverbdelayPanel.addRotarySlider(nullptr, "", Rectangle<int>(578 * resizeCoeff, 383 * resizeCoeff, rotarybg.getWidth() * resizeCoeff, rotarybg.getHeight() * resizeCoeff), "time", Image());
    reverbdelayPanel.addRotarySlider(nullptr, "", Rectangle<int>(295 * resizeCoeff, 383 * resizeCoeff, rotarybg.getWidth() * resizeCoeff, rotarybg.getHeight() * resizeCoeff), "feedback", Image());
    reverbdelayPanel.addButton(nullptr, "", Rectangle<int>(552 * resizeCoeff, -4 * resizeCoeff, buttonactiveimage.getWidth() * resizeCoeff, buttonactiveimage.getHeight() * resizeCoeff), "delayOnOff", Image());
    addAndMakeVisible(reverbdelayPanel);
    reverbdelayPanel.setBounds(1219 * resizeCoeff, 423 * resizeCoeff, 735 * resizeCoeff, 600 * resizeCoeff);

    // Functional Panel Initializaton
    funcPanel.addRotarySlider(nullptr, "", Rectangle<int>(42 * resizeCoeff, 0 * resizeCoeff, rotarybg.getWidth() * resizeCoeff, rotarybg.getHeight() * resizeCoeff), "func1", Image());
    funcPanel.addButton(nullptr, "", Rectangle<int>(10 * resizeCoeff, 115 * resizeCoeff, buttonactiveimage.getWidth() * resizeCoeff, buttonactiveimage.getHeight() * resizeCoeff), "func1OnOff", Image());
    funcPanel.addRotarySlider(nullptr, "", Rectangle<int>(296 * resizeCoeff, 0 * resizeCoeff, rotarybg.getWidth() * resizeCoeff, rotarybg.getHeight() * resizeCoeff), "func2", Image());
    funcPanel.addButton(nullptr, "", Rectangle<int>(261 * resizeCoeff, 115 * resizeCoeff, buttonactiveimage.getWidth() * resizeCoeff, buttonactiveimage.getHeight() * resizeCoeff), "func2OnOff", Image());
    funcPanel.addRotarySlider(nullptr, "", Rectangle<int>(563 * resizeCoeff, 0 * resizeCoeff, rotarybg.getWidth() * resizeCoeff, rotarybg.getHeight() * resizeCoeff), "func3", Image());
    funcPanel.addButton(nullptr, "", Rectangle<int>(534 * resizeCoeff, 115 * resizeCoeff, buttonactiveimage.getWidth() * resizeCoeff, buttonactiveimage.getHeight() * resizeCoeff), "func3OnOff", Image());
    funcPanel.addRotarySlider(nullptr, "", Rectangle<int>(843 * resizeCoeff, 0 * resizeCoeff, rotarybg.getWidth() * resizeCoeff, rotarybg.getHeight() * resizeCoeff), "func4", Image());
    funcPanel.addButton(nullptr, "", Rectangle<int>(816 * resizeCoeff, 115 * resizeCoeff, buttonactiveimage.getWidth() * resizeCoeff, buttonactiveimage.getHeight() * resizeCoeff), "func4OnOff", Image());
    funcPanel.addRotarySlider(nullptr, "", Rectangle<int>(1123 * resizeCoeff, 0 * resizeCoeff, rotarybg.getWidth() * resizeCoeff, rotarybg.getHeight() * resizeCoeff), "func5", Image());
    funcPanel.addButton(nullptr, "", Rectangle<int>(1090 * resizeCoeff, 115 * resizeCoeff, buttonactiveimage.getWidth() * resizeCoeff, buttonactiveimage.getHeight() * resizeCoeff), "func5OnOff", Image());
    addAndMakeVisible(funcPanel);
    funcPanel.setBounds(675 * resizeCoeff, 135 * resizeCoeff, 1277 * resizeCoeff, 171 * resizeCoeff);

    if (processor->getSynth())
        if (processor->getSynth()->getSound())
            attachElements();

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
    SummedSound *sound = processor->getSynth()->getSound();
    
    // Link Main Panel
    mainPanel.setLink(sound->rack->findEffect("volume"), "value", "volume");
    mainPanel.setLink(sound->rack->findEffect("pan"), "value", "pan");

    // Link ADSR Panel
    adsrPanel.setLink(sound->rack->findEffect("adsr"), "attack", "attack");
    adsrPanel.setLink(sound->rack->findEffect("adsr"), "decay", "decay");
    adsrPanel.setLink(sound->rack->findEffect("adsr"), "sustain", "sustain");
    adsrPanel.setLink(sound->rack->findEffect("adsr"), "release", "release");
    adsrPanel.setLink(sound->rack->findEffect("adsr"), "turnedOn", "onOff");

    // Link Reverb Panel
    reverbdelayPanel.setLink(sound->rack->findEffect("reverb"), "dryWet", "reverbDryWet");
    reverbdelayPanel.setLink(sound->rack->findEffect("reverb"), "roomSize", "roomSize");
    reverbdelayPanel.setLink(sound->rack->findEffect("reverb"), "width", "width");
    reverbdelayPanel.setLink(sound->rack->findEffect("reverb"), "turnedOn", "reverbOnOff");

    // Link Delay Panel
    reverbdelayPanel.setLink(sound->rack->findEffect("delay"), "dryWet", "delayDryWet");
    reverbdelayPanel.setLink(sound->rack->findEffect("delay"), "time", "time");
    reverbdelayPanel.setLink(sound->rack->findEffect("delay"), "feedback", "feedback");
    reverbdelayPanel.setLink(sound->rack->findEffect("delay"), "turnedOn", "delayOnOff");

    // Link Functional Panel
    funcPanel.setLink(sound->rack->findEffect("func"), "value", "func1");
    funcPanel.setLink(sound->rack->findEffect("func"), "turnedOn", "func1OnOff");
}

void rmpAudioProcessorEditor::paint (Graphics&)
{

}

void rmpAudioProcessorEditor::resized()
{
}
