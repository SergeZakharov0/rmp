/*
  ==============================================================================

    rmpSynth.cpp
    Created: 12 Jan 2019 1:24:52pm
    Author:  serge

  ==============================================================================
*/

#include "rmpSynth.h"
#include <math.h>

void VelocityBasedSynthesiser::noteOn (const int midiChannel,
                                       const int midiNoteNumber,
                                       const float velocity)
{
    const ScopedLock sl (lock);

    for (auto* s : sounds)
    {
        VelocityBasedSound* sound = static_cast<VelocityBasedSound*> (s);
        if (!sound)
            continue;
        if (sound->appliesToNoteAndVelocity (midiNoteNumber, velocity) && sound->appliesToChannel (midiChannel))
        {
            // If hitting a note that's still ringing, stop it first (it could be
            // still playing because of the sustain or sostenuto pedal).
            for (auto* voice : voices)
                if (voice->getCurrentlyPlayingNote() == midiNoteNumber && voice->isPlayingChannel (midiChannel))
                    stopVoice (voice, 1.0f, true);

            startVoice (findFreeVoice (sound, midiChannel, midiNoteNumber, isNoteStealingEnabled()),
                        sound, midiChannel, midiNoteNumber, velocity);
        }
    }
}

LayeredSamplesSound::LayeredSamplesSound() {
    for (int note = 0; note < 128; ++note) 
        for (int vel = 0; vel < 128; ++vel) 
			fullDataLength[note][vel] = 0;
    }
    
LayeredSamplesSound::LayeredSamplesSound(const char *config_file) {
    FILE *config = fopen(config_file, "r+");
    int lowNote, topNote, currentNote;
    int lowVel, topVel, currentVel;
    char filename[128];
    WavAudioFormat wav_decoder;
	for (int note = 0; note < 128; ++note)
		for (int vel = 0; vel < 128; ++vel)
			fullDataLength[note][vel] = 0;
   while (!feof(config)) {
	   fscanf(config, "%s %d %d %d %d %d %d\n", filename, &lowNote, &topNote, &currentNote, &lowVel, &topVel, &currentVel);
	   FileInputStream *input_stream = new FileInputStream(File(String(filename)));
        AudioFormatReader *source = wav_decoder.createReaderFor(input_stream, false);
        std::shared_ptr< AudioBuffer<float> > temp_pointer;
        
        int length = source->lengthInSamples;
        temp_pointer.reset (new AudioBuffer<float> (2, length + 4));
        source->read (temp_pointer.get(), 0, length + 4, 0, true, true);
        
        for (int stepNote = lowNote; stepNote <= topNote; ++stepNote)
			for (int stepVel = lowVel; stepVel <= topVel; ++stepVel) {
				fullData[stepNote][stepVel] = temp_pointer;
				fullDataLength[stepNote][stepVel] = length;
				}
        
        delete input_stream;
        
        }
    fclose(config);
    
}
    
LayeredSamplesSound::~LayeredSamplesSound() {}
    
    
bool LayeredSamplesSound::appliesToNote(int midiNoteNumber) {
    for (int vel = 0 ; vel < 128; ++vel)
        if (fullDataLength[midiNoteNumber][vel])
            return true;
    return false;
    }
    
bool LayeredSamplesSound::appliesToNoteAndVelocity(int midiNoteNumber, float velocity) {
	int vel = velocity * 128;
	int datalen = fullDataLength[midiNoteNumber][vel];
    return (datalen) ? true : false;
    }
    
bool LayeredSamplesSound::appliesToChannel(int) {
    return true;}
    
void rmpSynth::renderVoices (AudioBuffer<float>& buffer, int startSample, int numSamples) 
{
    for (auto* voice : voices)
        voice->renderNextBlock (buffer, startSample, numSamples);
    
    rack.applyEffects (buffer, startSample, numSamples);
}


LayeredSamplesVoice::LayeredSamplesVoice() {}
LayeredSamplesVoice::~LayeredSamplesVoice() {}

bool LayeredSamplesVoice::canPlaySound (SynthesiserSound *sound) 
{
    return dynamic_cast<const LayeredSamplesSound*> (sound) != nullptr;
}

void LayeredSamplesVoice::startNote (int midiNoteNumber, float velocity, SynthesiserSound *s, int currentPitchWheelPosition)
{
    if (auto* sound = dynamic_cast<const LayeredSamplesSound*> (s))
    {
        currentMidiNoteNumber = midiNoteNumber;
        currentVelocity = velocity;
        currentSamplePosition = 0;
    }
    else
    {
        jassertfalse; // this object can only play EffectLayerSound!
    }   
}

void LayeredSamplesVoice::stopNote (float /*velocity*/, bool allowTailOff)
{
    currentMidiNoteNumber = -1;
    currentVelocity = -1;
    currentSamplePosition = -1;
    clearCurrentNote();
}

template <typename floatType>
void LayeredSamplesVoice::_renderNextBlock (AudioBuffer<floatType>& outputBuffer, int startSample, int numSamples)
{
    if (auto* playingSound = static_cast<LayeredSamplesSound*> (getCurrentlyPlayingSound().get()))
    {
        auto& data   = *playingSound->getData(currentMidiNoteNumber, currentVelocity);
        int datasize = playingSound->getDataLength(currentMidiNoteNumber, currentVelocity);
        numSamples = ((numSamples + currentSamplePosition) > datasize) ? datasize - currentSamplePosition : numSamples;

        if (numSamples)
            if (data.getNumChannels() > 1 && outputBuffer.getNumChannels() > 1) 
            {
                outputBuffer.copyFrom(0, startSample, data, 0, currentSamplePosition, numSamples);
                outputBuffer.copyFrom(1, startSample, data, 1, currentSamplePosition, numSamples);
            }
            else if (data.getNumChannels() == 1)
            {
                outputBuffer.copyFrom(0, startSample, data, 0, currentSamplePosition, numSamples);
                outputBuffer.copyFrom(1, startSample, data, 0, currentSamplePosition, numSamples);
            }
            else if (outputBuffer.getNumChannels() == 1)
            {
                outputBuffer.copyFrom(0, startSample, data, 0, currentSamplePosition, numSamples);
                outputBuffer.applyGain(0, startSample, numSamples, 0.5);
                outputBuffer.addFrom(0, startSample, data, 1, currentSamplePosition, numSamples, 0.5);            
            }
        currentSamplePosition += numSamples;
    }
}

void LayeredSamplesVoice::pitchWheelMoved (int /*newValue*/) {}
void LayeredSamplesVoice::controllerMoved (int /*controllerNumber*/, int /*newValue*/) {}

void LayeredSamplesVoice::renderNextBlock (AudioBuffer<float>& outputBuffer, int startSample, int numSamples) {
    _renderNextBlock<float>(outputBuffer, startSample, numSamples);
}