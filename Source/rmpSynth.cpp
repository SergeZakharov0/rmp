/*
  ==============================================================================

    rmpSynth.cpp
    Created: 12 Jan 2019 1:24:52pm
    Author:  serge

  ==============================================================================
*/

#include "rmpSynth.h"
#include "PitchShifter.h"
#include <math.h>
#include <stdlib.h>
  

void LayeredSamplesSound::appendBox(soundBox &tempBox, float hostSampleRate) {

    WavAudioFormat wav_decoder;
    MemoryInputStream *input_stream = new MemoryInputStream((const void *)tempBox.soundfile_data, tempBox.soundfile_size, true);
    AudioFormatReader *source = wav_decoder.createReaderFor(input_stream, false);

    std::shared_ptr< AudioBuffer<float> > temp_pointer;

    double ratio = source->sampleRate / hostSampleRate;

    AudioBuffer<float> base(2, (int)source->lengthInSamples);
    source->read(&base, 0, (int)source->lengthInSamples, 0, true, true);

    int length = (int)(((float)base.getNumSamples()) / ratio);
    temp_pointer.reset(new AudioBuffer<float>(2, length));
    resample(base, *temp_pointer, (float)ratio);

    PitchShifter pitch_shifter;

    for (int stepNote = tempBox.lowestNote; stepNote <= tempBox.highestNote; ++stepNote) {
        std::shared_ptr< AudioBuffer<float> > transposed = pitch_shifter.transposeBuffer(temp_pointer, stepNote - tempBox.mainNote);
        std::shared_ptr< AudioBuffer<float> > prev = 0;
        for (int stepVel = tempBox.lowestVel; stepVel <= tempBox.highestVel; ++stepVel) {
            if (!fullData[stepNote][stepVel]) {
                fullData[stepNote][stepVel] = transposed;
                prev = 0;
            }
            else {
                if (prev == fullData[stepNote][stepVel]) {
                    continue;
                }
                prev = fullData[stepNote][stepVel];
                if (fullData[stepNote][stepVel]->getNumSamples() < transposed->getNumSamples()) {
                    fullData[stepNote][stepVel]->setSize(fullData[stepNote][stepVel]->getNumChannels(),
                        transposed->getNumSamples(), true, true);
                }

                fullData[stepNote][stepVel]->addFrom(0, 0, *transposed, 0, 0, transposed->getNumSamples());
                fullData[stepNote][stepVel]->addFrom(1, 0, *transposed, 1, 0, transposed->getNumSamples());

            }
        }
    }
    delete source;
}

LayeredSamplesSound::LayeredSamplesSound(XmlElement *layer_item, SQLInputSource *source, float hostSampleRate, std::list<LockingVoice *> &voices) {
	this->clear();

	forEachXmlChildElement(*layer_item, box_level_item) {
		if (box_level_item->hasTagName("name"))
			this->name = box_level_item->getAllSubText();
		if (box_level_item->hasTagName("box")) {
			soundBox tempBox;
			forEachXmlChildElement(*box_level_item, params_item) {
				if (params_item->hasTagName("mainnote"))
					tempBox.mainNote = (uint8)params_item->getAllSubText().getIntValue();
				if (params_item->hasTagName("lowestnote"))
					tempBox.lowestNote = (uint8)params_item->getAllSubText().getIntValue();
				if (params_item->hasTagName("highestnote"))
					tempBox.highestNote = (uint8)params_item->getAllSubText().getIntValue();
				if (params_item->hasTagName("mainvel"))
					tempBox.mainVel = (uint8)params_item->getAllSubText().getIntValue();
				if (params_item->hasTagName("lowestvel"))
					tempBox.lowestVel = (uint8)params_item->getAllSubText().getIntValue();
				if (params_item->hasTagName("highestvel"))
					tempBox.highestVel = (uint8)params_item->getAllSubText().getIntValue();
				if (params_item->hasTagName("transpose"))
					tempBox.transposeMethod = params_item->getAllSubText();
				if (params_item->hasTagName("soundfile")) {
					String soundfile = String(params_item->getAllSubText());
					MemoryInputStream *stream = (MemoryInputStream *)source->createInputStreamFor(soundfile);
					tempBox.soundfile_size = stream->getDataSize();
					tempBox.soundfile_data = malloc(tempBox.soundfile_size);
					memcpy(tempBox.soundfile_data, stream->getData(), tempBox.soundfile_size);
					delete stream;
					}
			}
			this->appendBox(tempBox, hostSampleRate);
		}	
        if (box_level_item->hasTagName("effects"))
        {
            rack.parseConfig(box_level_item, voices);
        }
	}
}
   
void LayeredSamplesSound::resample(AudioBuffer<float> &base, AudioBuffer<float> &resampled, float ratio) {
    ScopedPointer<LagrangeInterpolator> resampler = new LagrangeInterpolator();
    
    const float **inputs  = base.getArrayOfReadPointers();
    float **outputs = resampled.getArrayOfWritePointers();
    for (int c = 0; c < resampled.getNumChannels(); c++)
    {
	    resampler->reset();
	    resampler->process(ratio, inputs[c], outputs[c], resampled.getNumSamples());
    }
}
    
bool LayeredSamplesSound::appliesToNote(int midiNoteNumber) {
    for (int vel = 0 ; vel < 128; ++vel)
        if (getDataLength(midiNoteNumber, ((float)vel)/128))
            return true;
    return false;
    }
    
bool LayeredSamplesSound::appliesToNoteAndVelocity(int midiNoteNumber, float velocity) {
    return (getDataLength(midiNoteNumber, velocity)) ? true : false;
    }
    
bool LayeredSamplesSound::appliesToChannel(int) {
    return true;}
 
void LayeredSamplesSound::clear() {
	for (int note = 0; note < 128; ++note)
		for (int vel = 0; vel < 128; ++vel) {
			fullData[note][vel] = 0;
			}
}

bool SummedLayersVoice::canPlaySound (SynthesiserSound *sound)
{
    return dynamic_cast<const SummedLayersSound*> (sound) != nullptr;
}

void SummedLayersVoice::startNote(int, float velocity, SynthesiserSound *sound, int)
{
    if (SummedLayersSound* s = dynamic_cast<SummedLayersSound*> (sound))
    {
        currentlyPlayingVelocity = velocity;
        currentlyPlayingSamplePosition = 0;
        sendToListenersAboutStart();
    }
}

void SummedLayersVoice::stopNote(float, bool allowTailOff)
{
    if (askListenersForRelease() || !allowTailOff) {
        currentlyPlayingVelocity = -1;
        currentlyPlayingSamplePosition = 0;
        clearCurrentNote();
    }
};

template <typename floatType>
void SummedLayersVoice::_renderNextBlock (AudioBuffer<floatType>& outputBuffer, int startSample, int numSamples)
{
    if (auto* playingSound = static_cast<SummedLayersSound*> (getCurrentlyPlayingSound().get()))
    {
        int max_numsamples = 0;
        AudioBuffer<floatType> voice_buffer(outputBuffer.getNumChannels(), outputBuffer.getNumSamples());
        voice_buffer.clear();
        for (std::list<LayeredSamplesSound>::iterator current_sound = playingSound->layers.begin(); current_sound != playingSound->layers.end(); ++current_sound)
        {
            AudioBuffer<floatType> *pure_data = current_sound->getData(getCurrentlyPlayingNote(), currentlyPlayingVelocity).get();
            if (pure_data == nullptr)
                return;
            int datasize = current_sound->getDataLength(getCurrentlyPlayingNote(), currentlyPlayingVelocity);
            int current_numsamples = ((numSamples + currentlyPlayingSamplePosition) > datasize) ? datasize - currentlyPlayingSamplePosition : numSamples;

            if (current_numsamples <= 0)
                continue;

            if (max_numsamples < current_numsamples)
                max_numsamples = current_numsamples;

            AudioBuffer<floatType> aftereffect(*pure_data);
            current_sound->rack.applyOn(aftereffect, this, currentlyPlayingSamplePosition, current_numsamples);
                
            if (aftereffect.getNumChannels() > 1 && voice_buffer.getNumChannels() > 1)
            {
                voice_buffer.addFrom(0, startSample, aftereffect, 0, currentlyPlayingSamplePosition, current_numsamples);
                voice_buffer.addFrom(1, startSample, aftereffect, 1, currentlyPlayingSamplePosition, current_numsamples);
            }
            else if (aftereffect.getNumChannels() == 1)
            {
                voice_buffer.addFrom(0, startSample, aftereffect, 0, currentlyPlayingSamplePosition, current_numsamples);
                voice_buffer.addFrom(1, startSample, aftereffect, 0, currentlyPlayingSamplePosition, current_numsamples);
            }
            else if (voice_buffer.getNumChannels() == 1)
            {
                aftereffect.applyGain(0, startSample, current_numsamples, 0.5);
                voice_buffer.addFrom(0, startSample, aftereffect, 0, currentlyPlayingSamplePosition, current_numsamples);
                voice_buffer.addFrom(0, startSample, aftereffect, 1, currentlyPlayingSamplePosition, current_numsamples, 0.5);
            }         
        }

        playingSound->rack.applyOn(voice_buffer, this, startSample, max_numsamples);
        if (outputBuffer.getNumChannels() > 1 && voice_buffer.getNumChannels() > 1)
        {
            outputBuffer.addFrom(0, 0, voice_buffer, 0, 0, outputBuffer.getNumSamples());
            outputBuffer.addFrom(1, 0, voice_buffer, 1, 0, outputBuffer.getNumSamples());
        }
        else if (voice_buffer.getNumChannels() == 1)
        {
            outputBuffer.addFrom(0, 0, voice_buffer, 0, 0, outputBuffer.getNumSamples());
            outputBuffer.addFrom(1, 0, voice_buffer, 0, 0, outputBuffer.getNumSamples());
        }
        else if (outputBuffer.getNumChannels() == 1)
        {
            voice_buffer.applyGain(0, 0, max_numsamples, 0.5);
            outputBuffer.addFrom(0, 0, voice_buffer, 0, 0, outputBuffer.getNumSamples());
            outputBuffer.addFrom(0, 0, voice_buffer, 1, 0, outputBuffer.getNumSamples(), 0.5);
        }
        currentlyPlayingSamplePosition += max_numsamples;
    }
}

void SummedLayersVoice::renderNextBlock (AudioBuffer<float>& outputBuffer, int startSample, int numSamples) {

    _renderNextBlock<float>(outputBuffer, startSample, numSamples);
}


void VelocityBasedSynthesiser::noteOn(const int midiChannel,
    const int midiNoteNumber,
    const float velocity)
{
    const ScopedLock sl(lock);
    for (auto* s : sounds)
    {
        VelocityBasedSound* sound = static_cast<VelocityBasedSound*> (s);
        if (!sound)
            continue;
        if (sound->appliesToNoteAndVelocity(midiNoteNumber, velocity) && sound->appliesToChannel(midiChannel))
        {
            for (auto* voice : voices)
                if (voice->getCurrentlyPlayingNote() == midiNoteNumber && voice->isPlayingChannel(midiChannel))
                    stopVoice(voice, 1.0f, true);

            startVoice(findFreeVoice(sound, midiChannel, midiNoteNumber, isNoteStealingEnabled()),
                sound, midiChannel, midiNoteNumber, velocity);
        }
    }
}

void rmpSynth::renderVoices(AudioBuffer<float>& buffer, int startSample, int numSamples)
{
    for (auto* voice : voices)
        voice->renderNextBlock(buffer, startSample, numSamples);
}