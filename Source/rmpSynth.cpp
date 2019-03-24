
#include "rmpSynth.h"
#include "PitchShifter.h"
#include <math.h>
#include <stdlib.h>
  

void LayerSound::appendBox(soundBox &tempBox, float hostSampleRate) {

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

void LayerSound::resample(AudioBuffer<float> &base, AudioBuffer<float> &resampled, float ratio) {
    ScopedPointer<LagrangeInterpolator> resampler = new LagrangeInterpolator();
    
    const float **inputs  = base.getArrayOfReadPointers();
    float **outputs = resampled.getArrayOfWritePointers();
    for (int c = 0; c < resampled.getNumChannels(); c++)
    {
	    resampler->reset();
	    resampler->process(ratio, inputs[c], outputs[c], resampled.getNumSamples());
    }
}
    
bool LayerSound::appliesToNote(int midiNoteNumber) {
    for (int vel = 0 ; vel < 128; ++vel)
        if (getDataLength(midiNoteNumber, ((float)vel)/128))
            return true;
    return false;
    }
    
bool LayerSound::appliesToNoteAndVelocity(int midiNoteNumber, float velocity) {
    return (getDataLength(midiNoteNumber, velocity)) ? true : false;
    }
    
bool LayerSound::appliesToChannel(int) {
    return true;}
 
void LayerSound::clear() {
	for (int note = 0; note < 128; ++note)
		for (int vel = 0; vel < 128; ++vel) {
			fullData[note][vel] = 0;
			}
}

void LayerVoice::noteOn(int midiChannel, int midiNoteNumber, float velocity)
{
    currentPlayingMidiChannel = midiChannel;
    currentlyPlayingNote = midiNoteNumber;
    currentlyPlayingVelocity = velocity;
    currentSamplePosition = 0;
    sendToListenersAboutStart();
}

void LayerVoice::noteOff(bool forced)
{
    if (askListenersForRelease() || forced) 
    {
        currentPlayingMidiChannel = 0;
        currentlyPlayingNote = -1;
        currentlyPlayingVelocity = 0;
        currentSamplePosition = 0;
    }
};

void LayerVoice::renderNextBlock(AudioBuffer<float>& outputBuffer, int startSample, int numSamples) 
{
    if (isVoiceActive())
    {
        LayerSound &s = dynamic_cast<LayerSound&>(sound);
        AudioBuffer<float> *pure_data = s.getData(currentlyPlayingNote, currentlyPlayingVelocity).get();

        int datasize = s.getDataLength(currentlyPlayingNote, currentlyPlayingVelocity);
        int samplesToCopy = ((numSamples + currentSamplePosition) > datasize) ? datasize - currentSamplePosition : numSamples;

        if (samplesToCopy <= 0)
            this->noteOff(false);

        aftereffect.copyFrom(0, 0, *pure_data, 0, currentSamplePosition, samplesToCopy);
        aftereffect.copyFrom(1, 0, *pure_data, 1, currentSamplePosition, samplesToCopy);
        rack->applyOn(aftereffect, 0, samplesToCopy);

        if (aftereffect.getNumChannels() > 1 && outputBuffer.getNumChannels() > 1)
        {
            outputBuffer.addFrom(0, startSample, aftereffect, 0, 0, samplesToCopy);
            outputBuffer.addFrom(1, startSample, aftereffect, 1, 0, samplesToCopy);
        }
        else if (aftereffect.getNumChannels() == 1)
        {
            outputBuffer.addFrom(0, startSample, aftereffect, 0, 0, samplesToCopy);
            outputBuffer.addFrom(1, startSample, aftereffect, 0, 0, samplesToCopy);
        }
        else if (outputBuffer.getNumChannels() == 1)
        {
            aftereffect.applyGain(0, 0, samplesToCopy, 0.5);
            outputBuffer.addFrom(0, startSample, aftereffect, 0, 0, samplesToCopy);
            outputBuffer.addFrom(0, startSample, aftereffect, 1, 0, samplesToCopy, 0.5);
        }
        currentSamplePosition += samplesToCopy;
    }
}

void SummedVoice::noteOn(int midiChannel, int midiNoteNumber, float velocity)
{
    currentPlayingMidiChannel = midiChannel;
    currentlyPlayingNote = midiNoteNumber;
    currentlyPlayingVelocity = velocity;
    currentSamplePosition = 0;
    for (auto it = layerVoices.begin(); it != layerVoices.end(); ++it)
        (*it)->noteOn(midiChannel, midiNoteNumber, velocity);
    sendToListenersAboutStart();

}

void SummedVoice::noteOff(bool forced)
{
    if (askListenersForRelease() || forced)
    {
        for (auto it = layerVoices.begin(); it != layerVoices.end(); ++it)
            (*it)->noteOff(forced);
        refreshPlayingStatus();
    }
};

void SummedVoice::refreshPlayingStatus()
{
    bool status = false;
    for (auto voice = layerVoices.begin(); voice != layerVoices.end(); ++voice)
        if ((*voice)->isVoiceActive())
            status = true;

    if (!status)
    {
        currentPlayingMidiChannel = 0;
        currentlyPlayingNote = -1;
        currentlyPlayingVelocity = 0;
        currentSamplePosition = 0;
        askListenersForRelease();
    }
}

void SummedVoice::renderNextBlock (AudioBuffer<float>& outputBuffer, int startSample, int numSamples)
{
   // SummedVoice is responsible only for MIDI info parsing
}

LayerVoice *SummedVoice::findVoice(LayerSound *ofSound)
{
    for (auto voice = layerVoices.begin(); voice != layerVoices.end(); ++voice)
        if (voice->get()->getSound() == ofSound)
            return voice->get();
    throw;
}

void rmpSynth::noteOn(const int midiChannel, const int midiNoteNumber, const float velocity)
{
    const ScopedLock sl(lock);
    if (sound->appliesToNoteAndVelocity(midiNoteNumber, velocity) && sound->appliesToChannel(midiChannel))
    {
        for (auto voice = voices.begin(); voice != voices.end(); ++voice)
            if (voice->get()->getCurrentlyPlayingNote() == midiNoteNumber && voice->get()->isPlayingChannel(midiChannel))
                voice->get()->noteOff(true);

        SummedVoice *current = findFreeVoice(midiChannel, midiNoteNumber, isNoteStealingEnabled());
        if (current)
            current->noteOn(midiChannel, midiNoteNumber, velocity);
    }
}

void rmpSynth::noteOff(const int midiChannel, const int midiNoteNumber, const float velocity)
{
    const ScopedLock sl(lock);

    for (auto voice = voices.begin(); voice != voices.end(); ++voice)
    {
        if (voice->get()->getCurrentlyPlayingNote() == midiNoteNumber && voice->get()->isPlayingChannel(midiChannel))
            voice->get()->noteOff(false);
    }
}

void rmpSynth::reset()
{
    const ScopedLock sl(lock);
    for (auto voice = voices.begin(); voice != voices.end(); ++voice)
        voice->get()->noteOff(true);
}

void rmpSynth::renderNextBlock(AudioBuffer<float>& outputAudio, const MidiBuffer& midiData, int startSample, int numSamples)
{
    const int targetChannels = outputAudio.getNumChannels();

    MidiBuffer::Iterator midiIterator(midiData);
    midiIterator.setNextSamplePosition(startSample);

    bool firstEvent = true;
    int midiEventPos;
    MidiMessage m;

    const ScopedLock sl(lock);

    while (numSamples > 0)
    {
        if (!midiIterator.getNextEvent(m, midiEventPos))
        {
            if (targetChannels > 0)
                renderVoices(outputAudio, startSample, numSamples);

            return;
        }

        const int samplesToNextMidiMessage = midiEventPos - startSample;

        if (samplesToNextMidiMessage >= numSamples)
        {
            if (targetChannels > 0)
                renderVoices(outputAudio, startSample, numSamples);

            handleMidiEvent(m);
            break;
        }

        if (samplesToNextMidiMessage < ((firstEvent && !subBlockSubdivisionIsStrict) ? 1 : minimumSubBlockSize))
        {
            handleMidiEvent(m);
            continue;
        }

        firstEvent = false;

        if (targetChannels > 0)
            renderVoices(outputAudio, startSample, samplesToNextMidiMessage);

        handleMidiEvent(m);
        startSample += samplesToNextMidiMessage;
        numSamples -= samplesToNextMidiMessage;
    }
    while (midiIterator.getNextEvent(m, midiEventPos))
        handleMidiEvent(m);
}

void rmpSynth::renderVoices(AudioBuffer<float>& buffer, int startSample, int numSamples)
{
    AudioBuffer<float> soundsumBuffer(buffer.getNumChannels(), buffer.getNumSamples());
    soundsumBuffer.clear();
    AudioBuffer<float> layersumBuffer(buffer.getNumChannels(), buffer.getNumSamples());
    for (auto layerSound = sound->layerSounds.begin(); layerSound != sound->layerSounds.end(); ++layerSound)
    {
        layersumBuffer.clear();
        for (auto sumVoice = voices.begin(); sumVoice != voices.end(); ++sumVoice)
        {
            LayerVoice *layerVoice = sumVoice->get()->findVoice(layerSound->get());
            layerVoice->renderNextBlock(layersumBuffer, startSample, numSamples);
            sumVoice->get()->refreshPlayingStatus();
        }
        layerSound->get()->rack->applyOn(layersumBuffer, startSample, numSamples);
        
        if (layersumBuffer.getNumChannels() > 1 && soundsumBuffer.getNumChannels() > 1)
        {
            soundsumBuffer.addFrom(0, startSample, layersumBuffer, 0, startSample, numSamples);
            soundsumBuffer.addFrom(1, startSample, layersumBuffer, 1, startSample, numSamples);
        }
        else if (layersumBuffer.getNumChannels() == 1)
        {
            soundsumBuffer.addFrom(0, startSample, layersumBuffer, 0, startSample, numSamples);
            soundsumBuffer.addFrom(1, startSample, layersumBuffer, 0, startSample, numSamples);
        }
        else if (soundsumBuffer.getNumChannels() == 1)
        {
            layersumBuffer.applyGain(0, startSample, numSamples, 0.5);
            soundsumBuffer.addFrom(0, startSample, layersumBuffer, 0, startSample, numSamples);
            soundsumBuffer.addFrom(0, startSample, layersumBuffer, 1, startSample, numSamples, 0.5);
        }
    }
    sound->rack->applyOn(soundsumBuffer, startSample, numSamples);
    
    if (soundsumBuffer.getNumChannels() > 1 && buffer.getNumChannels() > 1)
    {
        buffer.addFrom(0, startSample, soundsumBuffer, 0, startSample, numSamples);
        buffer.addFrom(1, startSample, soundsumBuffer, 1, startSample, numSamples);
    }
    else if (soundsumBuffer.getNumChannels() == 1)
    {
        buffer.addFrom(0, startSample, soundsumBuffer, 0, startSample, numSamples);
        buffer.addFrom(1, startSample, soundsumBuffer, 0, startSample, numSamples);
    }
    else if (buffer.getNumChannels() == 1)
    {
        soundsumBuffer.applyGain(0, startSample, numSamples, 0.5);
        buffer.addFrom(0, startSample, soundsumBuffer, 0, startSample, numSamples);
        buffer.addFrom(0, startSample, soundsumBuffer, 1, startSample, numSamples, 0.5);
    }
}

void rmpSynth::handleMidiEvent(const MidiMessage& m)
{
    const int channel = m.getChannel();

    if (m.isNoteOn())
    {
        noteOn(channel, m.getNoteNumber(), m.getFloatVelocity());
    }
    else if (m.isNoteOff())
    {
        noteOff(channel, m.getNoteNumber(), m.getFloatVelocity());
    }
    else if (m.isAllNotesOff() || m.isAllSoundOff())
    {
        for (auto voice = voices.begin(); voice != voices.end(); ++voice)
            voice->get()->noteOff(false);
    }
    else if (m.isPitchWheel())
    {
        const int wheelPos = m.getPitchWheelValue();
        lastPitchWheelValues[channel - 1] = wheelPos;
        handlePitchWheel(channel, wheelPos);
    }
    else if (m.isAftertouch())
    {
        handleAftertouch(channel, m.getNoteNumber(), m.getAfterTouchValue());
    }
    else if (m.isChannelPressure())
    {
        handleChannelPressure(channel, m.getChannelPressureValue());
    }
    else if (m.isController())
    {
        handleController(channel, m.getControllerNumber(), m.getControllerValue());
    }
    else if (m.isProgramChange())
    {
        handleProgramChange(channel, m.getProgramChangeNumber());
    }
}

SummedVoice* rmpSynth::findFreeVoice(int midiChannel, int midiNoteNumber, bool stealIfNoneAvailable)
{
    const ScopedLock sl(lock);
    for (auto voice = voices.begin(); voice != voices.end(); ++voice)
        if (!voice->get()->isVoiceActive())
            return voice->get();

    if (stealIfNoneAvailable)
        return findVoiceToSteal(midiChannel, midiNoteNumber);

    return nullptr;
}

SummedVoice* rmpSynth::findVoiceToSteal(int midiChannel, int midiNoteNumber)
{
    return nullptr;
}

