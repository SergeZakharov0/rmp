
#include "EffectRack.h"

void rmpReverb::applyOn(AudioBuffer<float> &buffer, int startSample, int numSamples)
{
    if (std::get<TupleValues::currentValue>(params["turnedOn"]) == 0)
        return;

    if (numSamples == -1)
        numSamples = buffer.getNumSamples();

    int numChannels = buffer.getNumChannels();
    if (numChannels != 2 && numChannels != 1)
        return;

    float *l_channel = buffer.getWritePointer(0, startSample);
    float *r_channel = (numChannels > 1) ? buffer.getWritePointer(1, startSample) : 0;

	if (numChannels == 1)
		reverb.processMono(l_channel, numSamples);
	else if (numChannels == 2)
		reverb.processStereo(l_channel, r_channel, numSamples);
}

void rmpADSR::applyOn(AudioBuffer<float> &buffer, int startSample, int numSamples)
{

    if (prevBufferStatus == true && !adsr.isActive())
        delayedFinish();
    prevBufferStatus = adsr.isActive();

    if (std::get<TupleValues::currentValue>(params["turnedOn"]) == 0)
        return;

    adsr.applyEnvelopeToBuffer(buffer, startSample, numSamples);
}

void rmpVolume::applyOn(AudioBuffer<float> &buffer, int startSample, int numSamples)
{
    buffer.applyGain(startSample, numSamples, getParamValue("value"));
}

void rmpPan::applyOn(AudioBuffer<float> &buffer, int startSample, int numSamples)
{
    buffer.applyGain(0, startSample, numSamples, 1 - getParamValue("value"));
    buffer.applyGain(1, startSample, numSamples, getParamValue("value"));
}
