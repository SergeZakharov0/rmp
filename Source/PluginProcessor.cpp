
#include "PluginProcessor.h"
#include "PluginEditor.h"

rmpAudioProcessor::rmpAudioProcessor() : AudioProcessor (BusesProperties().withOutput ("Output", AudioChannelSet::stereo(), true))
{
    numSamples = 0;
	
	File datafile = File::getCurrentWorkingDirectory().getChildFile(".rmpdata");
	if (!datafile.getSize()) {
		FileChooser myChooser("The rompler was not correctly installed. Please specify the library folder",
			File::getSpecialLocation(File::userHomeDirectory));
		if (myChooser.browseForDirectory())
			libraryPath = myChooser.getResult().getFullPathName();
		datafile.replaceWithText(libraryPath);
	}
	else
		libraryPath = datafile.loadFileAsString();


	synth.setCurrentPlaybackSampleRate(48000);
}

void rmpAudioProcessor::applyInstrumentConfig(String configName, XmlElement *config, SQLInputSource *source) {
	synth.clearSounds();
    synth.clearVoices();
	
	currentConfigName = configName;

    std::list<LockingVoice *> voicesCreated;
    for (int i = 0; i < 2; ++i) 
    {
        SummedLayersVoice *now = new SummedLayersVoice();
        voicesCreated.push_back((LockingVoice *)now);
        synth.addVoice(now);
    }

	synth.addSound(new SummedLayersSound(config, source, (float)synth.getSampleRate(), voicesCreated));
	}

void rmpAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    synth.setCurrentPlaybackSampleRate (sampleRate);
    numSamples = samplesPerBlock;
}

void rmpAudioProcessor::processBlock (AudioBuffer<float>& buffer, MidiBuffer& midiMessages)
{
    ScopedNoDenormals noDenormals;
    auto totalNumInputChannels  = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear (i, 0, buffer.getNumSamples());

    keyboardState.processNextMidiBuffer (midiMessages, 0, numSamples, true);
    synth.renderNextBlock (buffer, midiMessages, 0, numSamples); 
}

AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new rmpAudioProcessor();
}