
#include "PluginProcessor.h"
#include "PluginEditor.h"

rmpAudioProcessor::rmpAudioProcessor() : AudioProcessor (BusesProperties().withOutput ("Output", AudioChannelSet::stereo(), true))
{
    numSamples = 0;
	
	File datafile = File::getSpecialLocation(File::userHomeDirectory).getChildFile("hyperia.data");
	if (!datafile.getSize()) {
		FileChooser myChooser("The rompler was not correctly installed. Please specify the library folder",
			File::getSpecialLocation(File::userHomeDirectory));
		if (myChooser.browseForDirectory())
			libraryPath = myChooser.getResult().getFullPathName();
		datafile.replaceWithText(libraryPath);
	}
	else
		libraryPath = datafile.loadFileAsString();

    synth = nullptr;
}

void rmpAudioProcessor::applyInstrumentConfig(String configName, XmlElement *config, SQLInputSource *source) 
{
    if (currentConfigName == configName)
        return;

	currentConfigName = configName;
    currentConfig = config;
    currentSource = source;
    reloadSynth();
}

void rmpAudioProcessor::reloadSynth()
{
    if (currentConfigName == "")
        return;
        
    if (synth)
    {
        synth->lock.enter();
        delete(synth);
    }
    InstrBuilder builder(currentConfig, currentSource, sampleRate);
    synth = builder.parseInstr(4);
}

void rmpAudioProcessor::prepareToPlay (double newRate, int samplesPerBlock)
{
    numSamples = samplesPerBlock;
    sampleRate = newRate;
    if (synth)
        reloadSynth();
}

void rmpAudioProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
    ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    keyboardState.processNextMidiBuffer (midiMessages, 0, numSamples, true);
    if (synth)
        synth->renderNextBlock(buffer, midiMessages, 0, numSamples);
}

AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new rmpAudioProcessor();
}