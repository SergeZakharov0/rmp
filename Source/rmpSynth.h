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
#include "EffectRack.h"
#include "LockingVoice.h"
#include <algorithm>
#include "SQLInputSource.h"
#include <unordered_set>

struct soundBox {
    uint8 mainNote, lowestNote, highestNote;
    uint8 mainVel, lowestVel, highestVel;
    String transposeMethod;
    void *soundfile_data;
    size_t soundfile_size;
    soundBox() = default;
    ~soundBox() { free(soundfile_data); }
};

class VelocityBasedSound : public SynthesiserSound 
{
    public:
    virtual bool appliesToNoteAndVelocity (int midiNoteNumber, float velocity) = 0;
    private:

    JUCE_LEAK_DETECTOR (VelocityBasedSound)
};

class LayeredSamplesSound : public VelocityBasedSound 
{
public:
    LayeredSamplesSound(XmlElement *layer_item, SQLInputSource *source, float hostSampleRate, std::list<LockingVoice *> &voices);
    ~LayeredSamplesSound() = default;

    bool appliesToNote(int midiNoteNumber) override;
    bool appliesToNoteAndVelocity(int midiNoteNumber, float velocity);
    bool appliesToChannel (int midiChannel) override;

    std::shared_ptr< AudioBuffer<float> > getData(int currentMidiNoteNumber, float currentVelocity) 
    {
        return fullData[currentMidiNoteNumber][int(currentVelocity*128)]; 
    };
    int getDataLength(int midiNoteNumber, float velocity) 
    {
        return (fullData[midiNoteNumber][int(velocity * 128)]) ? fullData[midiNoteNumber][int(velocity * 128)]->getNumSamples() : 0;
    };

    rmpEffectRack rack;
protected:
    void appendBox(soundBox &tempBox, float hostSampleRate);
    void resample(AudioBuffer<float> &base, AudioBuffer<float> &resampled, float ratio);
	void clear();

    String name;
	std::shared_ptr< AudioBuffer<float> > fullData[128][128];
private:
    JUCE_LEAK_DETECTOR (LayeredSamplesSound)
};

class SummedLayersVoice : public LockingVoice
{
public:
    SummedLayersVoice() = default;
    ~SummedLayersVoice() = default;

    bool canPlaySound(SynthesiserSound *sound);
    void startNote(int midiNoteNumber, float velocity, SynthesiserSound *sound, int currentPitchWheelPosition);
    void stopNote(float velocity, bool allowTailOff);
    void renderNextBlock(AudioBuffer<float> &outputBuffer, int startSample, int numSamples) override;
    void pitchWheelMoved(int) {};
    void controllerMoved(int, int) {};

protected:
    float currentlyPlayingVelocity = 0;
    int currentlyPlayingSamplePosition = 0;
private:
    template <typename floatType>
    void _renderNextBlock(AudioBuffer<floatType>& outputBuffer, int startSample, int numSamples);

    JUCE_LEAK_DETECTOR(SummedLayersVoice)
};


class SummedLayersSound : public VelocityBasedSound {
public:
    SummedLayersSound() {};
    SummedLayersSound(XmlElement *main_element, SQLInputSource *source, float hostSampleRate, std::list<LockingVoice *> &voices)
    {
        forEachXmlChildElement(*main_element, layer_item) {
            if (layer_item->hasTagName("layer")) 
            {
                layers.emplace_back(layer_item, source, hostSampleRate, voices);
            }
            if (layer_item->hasTagName("effects")) 
            {
                rack.parseConfig(layer_item, voices);
            }
        }
    }
    ~SummedLayersSound() = default;

    bool appliesToNote(int midiNoteNumber) 
    {
        for (std::list<LayeredSamplesSound>::iterator it = layers.begin(); it != layers.end(); ++it)
            if (it->appliesToNote(midiNoteNumber))
                return true;
        return false;
    }
    bool appliesToNoteAndVelocity(int midiNoteNumber, float velocity)
    {
        for (std::list<LayeredSamplesSound>::iterator it = layers.begin(); it != layers.end(); ++it)
            if (it->appliesToNoteAndVelocity(midiNoteNumber, velocity))
                return true;
        return false;
    }
    bool appliesToChannel(int midiChannel)
    {
        for (std::list<LayeredSamplesSound>::iterator it = layers.begin(); it != layers.end(); ++it)
            if (it->appliesToChannel(midiChannel))
                return true;
        return false;
    }

    rmpEffectRack rack;
    std::list<LayeredSamplesSound> layers;
private:
    JUCE_LEAK_DETECTOR(SummedLayersSound)
};

class VelocityBasedSynthesiser : public Synthesiser
{
public:
    void noteOn(const int midiChannel, const int midiNoteNumber, const float velocity) override;
};

class rmpSynth : public VelocityBasedSynthesiser
{
public:
    void renderVoices(AudioBuffer<float>& buffer, int startSample, int numSamples) override;
};