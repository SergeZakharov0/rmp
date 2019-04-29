
#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include <list>
#include "EffectRack.h"
#include <algorithm>
#include "StartStopBroadcaster.h"
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

class rmpSound
{
public:
    rmpSound() = default;
    virtual ~rmpSound() = default;
    rmpSound(rmpSound &) = default;
    rmpSound(rmpSound &&) = default;

    virtual bool appliesToNote(int midiNoteNumber) = 0;
    virtual bool appliesToNoteAndVelocity(int midiNoteNumber, float velocity) = 0;
    virtual bool appliesToChannel(int midiChannel) = 0;
};

class LayerSound : public rmpSound
{
public:
    LayerSound() = default;
    ~LayerSound() = default;
    LayerSound(LayerSound &) = default;
    LayerSound(LayerSound &&) = default;

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

    std::shared_ptr<rmpEffectRack> rack;
protected:
    friend class InstrBuilder;
    void appendBox(soundBox &tempBox, float hostSampleRate);
    void resample(AudioBuffer<float> &base, AudioBuffer<float> &resampled, float ratio);
	void clear();

    String name;
	std::shared_ptr< AudioBuffer<float> > fullData[128][128];
};

class SummedSound : public rmpSound {
public:
    SummedSound() = default;
    ~SummedSound() = default;

    bool appliesToNote(int midiNoteNumber)
    {
        for (auto it = layerSounds.begin(); it != layerSounds.end(); ++it)
            if ((*it)->appliesToNote(midiNoteNumber))
                return true;
        return false;
    }
    bool appliesToNoteAndVelocity(int midiNoteNumber, float velocity)
    {
        for (auto it = layerSounds.begin(); it != layerSounds.end(); ++it)
            if ((*it)->appliesToNoteAndVelocity(midiNoteNumber, velocity))
                return true;
        return false;
    }
    bool appliesToChannel(int midiChannel)
    {
        for (auto it = layerSounds.begin(); it != layerSounds.end(); ++it)
            if ((*it)->appliesToChannel(midiChannel))
                return true;
        return false;
    }

    std::shared_ptr<rmpEffectRack> rack;
    std::list<std::shared_ptr<LayerSound>> layerSounds;
protected:
    String name;
};


class rmpVoice : public StartStopBroadcaster
{
public:
    rmpVoice(rmpSound &_sound) : sound(_sound) {};
    virtual ~rmpVoice() = default;
    rmpVoice(rmpVoice &) = default;
    rmpVoice(rmpVoice &&) = default;

    int getCurrentlyPlayingNote() const noexcept { return currentlyPlayingNote; }
    rmpSound *getSound() const noexcept { return &sound; }

    virtual void noteOn(int midiChannel, int midiNoteNumber, float velocity) = 0;
    virtual void noteOff(bool forced) = 0;

    bool isVoiceActive() const { return currentlyPlayingNote >= 0; };
    void reactOnDelayedStop() { noteOff(false); };

    virtual void pitchWheelMoved(int newPitchWheelValue) {};
    virtual void controllerMoved(int controllerNumber, int newControllerValue) {};
    virtual void aftertouchChanged(int newAftertouchValue) {};
    virtual void channelPressureChanged(int newChannelPressureValue) {};
    virtual void renderNextBlock(AudioBuffer<float>& outputBuffer, int startSample, int numSamples) = 0;

    virtual void setCurrentPlaybackSampleRate(double newRate) { currentSampleRate = newRate; };
    virtual bool isPlayingChannel(int midiChannel) const { return true; };

    double getSampleRate() const noexcept { return currentSampleRate; }

    bool isSustainPedalDown() const noexcept { return sustainPedalDown; }
    void setSustainPedalDown(bool isNowDown) noexcept { sustainPedalDown = isNowDown; }

    bool isSostenutoPedalDown() const noexcept { return sostenutoPedalDown; }
    void setSostenutoPedalDown(bool isNowDown) noexcept { sostenutoPedalDown = isNowDown; }

protected:
    rmpSound &sound;
    double currentSampleRate = 44100.0;
    int currentlyPlayingNote = -1, currentPlayingMidiChannel = 0, currentSamplePosition = 0;
    float currentlyPlayingVelocity = 0;
    bool keyIsDown = false, sustainPedalDown = false, sostenutoPedalDown = false;
    friend class InstrBuilder;
};

class LayerVoice : public rmpVoice
{
public:
    LayerVoice(LayerSound &_sound) : rmpVoice(_sound) 
    {
        aftereffect.setSize(2, 256);

    };
    ~LayerVoice() = default;
    LayerVoice(LayerVoice &) = default;
    LayerVoice(LayerVoice &&) = default;

    void noteOn(int midiChannel, int midiNoteNumber, float velocity) override;
    void noteOff(bool forced) override;
    void renderNextBlock(AudioBuffer<float> &outputBuffer, int startSample, int numSamples) override;

    void repairRackLinks()
    {
        clearListeners();
        StartStopBroadcaster::Listener *adsr = dynamic_cast<StartStopBroadcaster::Listener *>(rack->findEffect("adsr"));
        if (adsr)
            addListener(adsr);
    };

    std::shared_ptr<rmpEffectRack> rack;
protected:
    AudioBuffer<float> aftereffect;

};

class SummedVoice : public rmpVoice
{
public:
    SummedVoice(SummedSound &_sound) : rmpVoice(_sound) {};
    ~SummedVoice() = default;
    SummedVoice(SummedVoice &&) = default;

    void noteOn(int midiChannel, int midiNoteNumber, float velocity) override;
    void noteOff(bool forced) override;

    void refreshPlayingStatus();

    void renderNextBlock(AudioBuffer<float> &outputBuffer, int startSample, int numSamples) override;

    LayerVoice *findVoice(LayerSound *ofSound);
    void repairRackLinks()
    {
        clearListeners();
        StartStopBroadcaster::Listener *adsr = dynamic_cast<StartStopBroadcaster::Listener *>(rack->findEffect("adsr"));
        if (adsr)
            addListener(adsr);
    };

    std::shared_ptr<rmpEffectRack> rack;
    std::list<std::shared_ptr<LayerVoice>> layerVoices;
};

class rmpSynth
{
public:
    rmpSynth(CriticalSection &_lock)
    {
        soundsumBuffer.setSize(2, 256);
        layersumBuffer.setSize(2, 256);
    }
    ~rmpSynth() = default;
    rmpSynth(rmpSynth &&) = default;

    void clearVoices() { return voices.clear(); };
    int getNumVoices() const noexcept { return voices.size(); }
    
    SummedSound *getSound()
    {
        return sound.get();
    };

    void setNoteStealingEnabled(bool shouldSteal) { shouldStealNotes = shouldSteal; }
    bool isNoteStealingEnabled() const noexcept { return shouldStealNotes; }

    void noteOn(int midiChannel, int midiNoteNumber, float velocity);
    void noteOff(int midiChannel, int midiNoteNumber, float velocity);
    void reset();

    void handlePitchWheel(int midiChannel, int wheelValue) {};
    void handleController(int midiChannel, int controllerNumber, int controllerValue) {};
    void handleAftertouch(int midiChannel, int midiNoteNumber, int aftertouchValue) {};
    void handleChannelPressure(int midiChannel, int channelPressureValue) {};
    void handleSustainPedal(int midiChannel, bool isDown) {};
    void handleSostenutoPedal(int midiChannel, bool isDown) {};
    void handleSoftPedal(int midiChannel, bool isDown) {};
    void handleProgramChange(int midiChannel, int programNumber) {};

    void setCurrentPlaybackSampleRate(double rate) { sampleRate = rate; };
    double getSampleRate() const noexcept { return sampleRate; }

    void renderNextBlock(AudioBuffer<float>& outputAudio, const MidiBuffer& inputMidi, int startSample, int numSamples);
    void turnOff();

    void setMinimumRenderingSubdivisionSize(int numSamples, bool shouldBeStrict = false) noexcept
    {
        minimumSubBlockSize = numSamples;
        subBlockSubdivisionIsStrict = shouldBeStrict;
    }

    CriticalSection lock;
    bool turnedOff = false;
protected:

    friend class InstrBuilder;
    std::list<std::shared_ptr<SummedVoice>> voices;
    std::shared_ptr<SummedSound> sound;
    int lastPitchWheelValues[16];

    AudioBuffer<float> soundsumBuffer;
    AudioBuffer<float> layersumBuffer;

    void renderVoices(AudioBuffer<float>& outputAudio, int startSample, int numSamples);
    SummedVoice* findFreeVoice(int midiChannel, int midiNoteNumber, bool stealIfNoneAvailable);
    SummedVoice* findVoiceToSteal(int midiChannel, int midiNoteNumber);

    void handleMidiEvent(const MidiMessage&);

    double sampleRate = 0;
    uint32 lastNoteOnCounter = 0;
    int minimumSubBlockSize = 32;
    bool subBlockSubdivisionIsStrict = false;
    bool shouldStealNotes = true;
    BigInteger sustainPedalsDown;
};