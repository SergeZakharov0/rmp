/*
  ==============================================================================

    rmpSynth.h
    Created: 12 Jan 2019 1:25:03pm
    Author:  serge

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include <list>

class EffectRack {
    public:
    virtual void applyEffects(AudioBuffer<float>& buffer, int startSample, int numSamples) {};
};

class VelocityBasedSound : public SynthesiserSound 
{
    public:
    virtual bool appliesToNoteAndVelocity (int midiNoteNumber, float velocity) = 0;
    private:

    JUCE_LEAK_DETECTOR (VelocityBasedSound)
};

class VelocityBasedSynthesiser : public Synthesiser 
{
    public:
    void noteOn (const int midiChannel, const int midiNoteNumber, const float velocity) override;
};

class rmpSynth : public VelocityBasedSynthesiser 
{
    public:
        void renderVoices (AudioBuffer<float>& buffer, int startSample, int numSamples) override;
    protected:
        EffectRack rack;
};

class LayeredSamplesSound : public VelocityBasedSound 
{
    public:
    LayeredSamplesSound();
    LayeredSamplesSound(const char *config_file);
    ~LayeredSamplesSound();

    bool appliesToNote(int midiNoteNumber) override;
    bool appliesToNoteAndVelocity(int midiNoteNumber, float velocity);
    bool appliesToChannel (int midiChannel) override;
    
    std::shared_ptr< AudioBuffer<float> > getData(int currentMidiNoteNumber, float currentVelocity) {
        return fullData[currentMidiNoteNumber][int(currentVelocity*128)]; };
    int getDataLength(int midiNoteNumber, float velocity) {
        return fullDataLength[midiNoteNumber][int(velocity*128)]; };

    protected:
    std::shared_ptr< AudioBuffer<float> > fullData[128][128];
    unsigned int fullDataLength[128][128];

    private:
    JUCE_LEAK_DETECTOR (LayeredSamplesSound)
};

class LayeredSamplesVoice : public SynthesiserVoice
{
    public:
    LayeredSamplesVoice();
    
    ~LayeredSamplesVoice();
    bool canPlaySound (SynthesiserSound *) override;
    void startNote (int midiNoteNumber, float velocity, SynthesiserSound *sound, int currentPitchWheelPosition) override;
    void stopNote (float velocity, bool allowTailOff ) override;
    void pitchWheelMoved (int newValue) override;
    void controllerMoved (int controllerNumber, int newValue) override;
    void renderNextBlock (AudioBuffer<float> &outputBuffer, int startSample, int numSamples) override;

    protected:
    int currentMidiNoteNumber = 0;
    float currentVelocity = 0;
    int currentSamplePosition = 0;
    EffectRack rack;

    private:
    template <typename floatType>
    void _renderNextBlock (AudioBuffer<floatType>& outputBuffer, int startSample, int numSamples);

    JUCE_LEAK_DETECTOR (SamplerVoice)
};