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
            for (auto* voice : voices)
                if (voice->getCurrentlyPlayingNote() == midiNoteNumber && voice->isPlayingChannel (midiChannel))
                    stopVoice (voice, 1.0f, true);

            startVoice (findFreeVoice (sound, midiChannel, midiNoteNumber, isNoteStealingEnabled()),
                        sound, midiChannel, midiNoteNumber, velocity);
        }
    }
}

LayeredSamplesSound::LayeredSamplesSound() {
	this->clear();
    }
    
LayeredSamplesSound::LayeredSamplesSound(XmlElement *layer_item, SQLInputSource *source, float hostSampleRate) {
	this->clear();

	forEachXmlChildElement(*layer_item, box_level_item) {
		if (box_level_item->hasTagName("name"))
			this->name = box_level_item->getAllSubText();
		if (box_level_item->hasTagName("box")) {
			soundBox tempBox;
			forEachXmlChildElement(*box_level_item, params_item) {
				if (params_item->hasTagName("mainnote"))
					tempBox.mainNote = params_item->getAllSubText().getIntValue();
				if (params_item->hasTagName("lowestnote"))
					tempBox.lowestNote = params_item->getAllSubText().getIntValue();
				if (params_item->hasTagName("highestnote"))
					tempBox.highestNote = params_item->getAllSubText().getIntValue();
				if (params_item->hasTagName("mainvel"))
					tempBox.mainVel = params_item->getAllSubText().getIntValue();
				if (params_item->hasTagName("lowestvel"))
					tempBox.lowestVel = params_item->getAllSubText().getIntValue();
				if (params_item->hasTagName("highestvel"))
					tempBox.highestVel = params_item->getAllSubText().getIntValue();
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
	}
}

void LayeredSamplesSound::appendBox(soundBox &tempBox, float hostSampleRate) {
	
	WavAudioFormat wav_decoder;
	MemoryInputStream *input_stream = new MemoryInputStream((const void *)tempBox.soundfile_data, tempBox.soundfile_size, true);
	AudioFormatReader *source = wav_decoder.createReaderFor(input_stream, false);

	std::shared_ptr< AudioBuffer<float> > temp_pointer;

	double ratio = source->sampleRate / hostSampleRate;

	AudioBuffer<float> base(2, source->lengthInSamples + 4);
	source->read(&base, 0, source->lengthInSamples + 4, 0, true, true);

	int length = (int)((float)base.getNumSamples()) / ratio;
	temp_pointer.reset(new AudioBuffer<float>(2, length));
	resample(base, *temp_pointer, ratio);

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
					continue; }
				prev = fullData[stepNote][stepVel];
				if (fullData[stepNote][stepVel]->getNumSamples() < transposed->getNumSamples()) {
					fullData[stepNote][stepVel]->setSize(fullData[stepNote][stepVel]->getNumChannels(),
						transposed->getNumSamples(), true, true);
				}

				fullData[stepNote][stepVel]->addFrom(0, 0, *transposed, 0, 0, transposed->getNumSamples());
				fullData[stepNote][stepVel]->addFrom(1, 0, *transposed, 1, 0, transposed->getNumSamples());

			}
			fullDataLength[stepNote][stepVel] = fullData[stepNote][stepVel]->getNumSamples();
		}
	}
	delete source;
}
   

LayeredSamplesSound::~LayeredSamplesSound() {}

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
 
void LayeredSamplesSound::clear() {
	for (int note = 0; note < 128; ++note)
		for (int vel = 0; vel < 128; ++vel) {
			fullDataLength[note][vel] = 0;
			fullData[note][vel] = 0;
			}
}

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