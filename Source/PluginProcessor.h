#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "rmpSynth.h"
#include "SQLInputSource.h"
#include "PluginEditor.h"

class rmpAudioProcessor  : public AudioProcessor
{
public:
    rmpAudioProcessor();
    ~rmpAudioProcessor() = default;
    
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void applyInstrumentConfig(String configName, XmlElement *config, SQLInputSource *source);
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
    rmpSynth* getSynth() { return &synth; };

	String libraryPath;
	String currentConfigName;
private:
    rmpSynth synth;
    int numSamples;
    MidiKeyboardState keyboardState;
    static const int maxBufferSize = 2048;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (rmpAudioProcessor)
};
