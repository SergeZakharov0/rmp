#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "rmpSynth.h"
#include "SQLInputSource.h"
#include "PluginEditor.h"
#include "InstrBuilder.h"

class rmpAudioProcessor  : public AudioProcessor
{
public:
    rmpAudioProcessor();
    ~rmpAudioProcessor() 
    { 
        if (synth) 
            delete(synth); 
        if (prevSynth) 
            delete(prevSynth); 
        if (currentConfig)
            delete(currentConfig);
        if (currentSource)
            delete(currentSource);
    };
    
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void applyInstrumentConfig(String configName, XmlElement *config, SQLInputSource *source);
    void reloadSynth();
    void releaseResources() override {};

    bool isBusesLayoutSupported (const BusesLayout& layouts) const override
    {
        return (layouts.getMainOutputChannelSet() == AudioChannelSet::mono() || layouts.getMainOutputChannelSet() != AudioChannelSet::stereo());
    };

    void processBlock (AudioBuffer<float>&, MidiBuffer&) override;

    AudioProcessorEditor* createEditor() override { return new rmpAudioProcessorEditor(this); };
    bool hasEditor() const override { return true; }

    const String getName() const override { return "Hyperia"; };

    bool acceptsMidi() const override { return true; };
    bool producesMidi() const override { return false; };
    bool isMidiEffect() const override { return false; };
    double getTailLengthSeconds() const override { return 0.0; };

    int getNumPrograms() override { return 1; };
    int getCurrentProgram() override { return 0; };
    void setCurrentProgram(int) override { return; };
    const String getProgramName(int) override { return "Hyperia"; };
    void changeProgramName(int, const String&) override { return; };

    void getStateInformation(MemoryBlock&) override {};
    void setStateInformation(const void*, int) override {};

    MidiKeyboardState& getKBState() { return keyboardState; };
    rmpSynth* getSynth() { return synth; };
    void reset() { if (synth) synth->reset(); };

	String libraryPath;

    String currentConfigName = "";
    XmlElement *currentConfig = nullptr; 
    SQLInputSource *currentSource = nullptr;
private:
    CriticalSection lock;
    rmpSynth *synth = nullptr, *prevSynth = nullptr;

    float sampleRate = 0;
    int numSamples = 0;
    MidiKeyboardState keyboardState;
    static const int maxBufferSize = 2048;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (rmpAudioProcessor)
};
