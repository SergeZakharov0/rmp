/*
  ==============================================================================

    EffectRack.cpp
    Created: 23 Feb 2019 5:44:28pm
    Author:  CodyDog

  ==============================================================================
*/

#include "EffectRack.h"

//========================================================================
rmpEffect::rmpEffect()
{

}

rmpEffect::~rmpEffect()
{

}

void rmpEffect::applyEffect(AudioBuffer<float> &buffer)
{

}

//============================================================================
rmpReverb::rmpReverb()
{

}

rmpReverb::~rmpReverb()
{

}

void rmpReverb::applyEffect(AudioBuffer<float> &buffer)
{
	float *l_channel = buffer.getWritePointer(0);
	float *r_channel = buffer.getWritePointer(1);
	int numSamples = buffer.getNumSamples();

	reverb.processStereo(l_channel, r_channel, numSamples);
}

//=============================================================================

rmpADSR::rmpADSR()
{
	startSample = 0;
	endSample = 0;
}

rmpADSR::~rmpADSR()
{

}

void rmpADSR::applyEffect(AudioBuffer<float> &buffer)
{
	if (endSample == 0)
	{
		endSample = buffer.getNumSamples();
	}
	//adsr.applyEnvelopeToBuffer(buffer, startSample, endSample);
}


EffectRack::EffectRack()
{
	//effectRack.push_back(adsr);
	effectRack.push_back(reverb);

}

EffectRack::~EffectRack()
{
    
}

void EffectRack::applyEffects(AudioBuffer<float> &buffer)
{
	for(ptr = effectRack.begin(); ptr < effectRack.end(); ++ptr)
	{
		ptr->applyEffect(buffer);
	}
    
}

//==================================================




