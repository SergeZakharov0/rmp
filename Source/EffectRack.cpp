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
rmpReverb::rmpReverb( const double sampleRate)
{
	
	params.dryWet = 0.5f;
	params.depth = 0.5f;
	params.width = 0.5f;

	setSampleRate(sampleRate);
	setParams(params);
	
}

rmpReverb::~rmpReverb()
{

}

void rmpReverb::setSampleRate(const double sampleRate)
{
	reverb.setSampleRate(sampleRate);
}

void rmpReverb::setParams()
{
	Reverb::Parameters rparams;
	rparams.dryLevel = 1.0f - params.dryWet;
	rparams.wetLevel = params.dryWet;
	rparams.roomSize = params.depth;
	rparams.width = params.width;

	reverb.setParameters(rparams);
}

void rmpReverb::setParams( ReverbParams parameters )
{
	params = parameters;
	setParams();
	
}

ReverbParams rmpReverb::getParams()
{
	return params;
}

void rmpReverb::applyEffect(AudioBuffer<float> &buffer)
{
	float *l_channel = buffer.getWritePointer(0);
	float *r_channel = buffer.getWritePointer(1);
	int numSamples = buffer.getNumSamples();

	reverb.processStereo(l_channel, r_channel, numSamples);
}

void rmpReverb::setSingleParam(ReverbParam param, float val)
{
	switch (param)
	{
	    case epDryWet:
			params.dryWet = val;
			break;

		case epDepth:
			params.depth = val;
			break;

		case epWidth:
			params.width = val;
			break;

		default:
			break;
	}
}

//=============================================================================

rmpADSR::rmpADSR()
{
	startSample = 0;
	endSample = 0;

	params.attack = 0.1f;
	params.decay = 0.5f;
	params.release = 0.5f;
	params.sustain = 1.0f;

	adsr.setSampleRate(48000);

	setParams();
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
	adsr.applyEnvelopeToBuffer(buffer, startSample, endSample);
}

AdsrParams rmpADSR::getParams()
{
	return params;
}

void rmpADSR::setParams()
{
	ADSR::Parameters aparams;

	aparams.attack = params.attack;
	aparams.decay = params.decay;
	aparams.release = params.release;
	aparams.sustain = params.sustain;

	adsr.setParameters(aparams);
}

void rmpADSR::setParams(AdsrParams parameters)
{
	params = parameters;
	setParams();

}

void rmpADSR::setSingleParam(AdsrParam param, float val)
{
	switch (param)
	{
	    case epAttack:
			params.attack = val;
			break;

		case epDecay:
			params.decay = val;
			break;

		case epSustain:
			params.sustain = val;
			break;
			
		case epRelease:
			params.release = val;
			break;

		default:
			break;
	}
}

//=======================================================================
EffectRack::EffectRack(const double sampleRate)
{
	reverb = new rmpReverb(sampleRate);
	adsr = new rmpADSR();
	effectRack.push_back(*adsr);
	effectRack.push_back(*reverb);
	volume = 100;
	pan = 0;
	for (int i = 0; i < rmpEffects::commonEffectsVal; i++)
	{
		effectIsOff[i] = false;
	}

	effectIsOff[rmpEffects::adsr] = true;

}

EffectRack::~EffectRack()
{
	delete reverb;
}

void EffectRack::onOffEffect(int id)
{
	effectIsOff[id] = !effectIsOff[id];

}

void EffectRack::applyEffects(AudioBuffer<float> &buffer)
{
	
	for(int effect = 0; effect < rmpEffects::commonEffectsVal; ++effect)
	{
		if (effectIsOff[effect])
		{
			continue;
		}

		switch (effect)
		{
		case rmpEffects::adsr:
			adsr->applyEffect(buffer);
			break;

		case rmpEffects::reverb:
			reverb->applyEffect(buffer);
			break;

		default:
			break;
		}
		
	}
	
	
	applyVolToBuffer(buffer);


}

void EffectRack::applyVolToBuffer(AudioBuffer<float>& buffer)
{
	auto *buff_ptr0 = buffer.getWritePointer(0); //Left channel buffer
	auto *buff_ptr1 = buffer.getWritePointer(1); //Right channel buffer
	int numSamples = buffer.getNumSamples();

	float panVal = (pan + 100.0f) * 0.005f;
	float volVal = volume * 0.01f;
	float gainCh0 = volVal * (1.0f - panVal);
	float gainCh1 = volVal * panVal;
	for (auto sample = 0; sample < numSamples; ++sample)
	{
		buff_ptr0[sample] *= gainCh0;
		buff_ptr1[sample] *= gainCh1;
	}

}

void EffectRack::volValChanged(float val)
{
	volume = val;

}

void EffectRack::panValChanged(float val)
{
	pan = val;
}

void EffectRack::sliderValueChanged(Slider* slider)
{
	if (slider->getName().compare("pan") == 0)
	{
		panValChanged(slider->getValue());
	}
	else if (slider->getName().compare("volume") == 0)
	{
		volValChanged(slider->getValue());
	}
}

void EffectRack::mouseDown(const MouseEvent &event)
{
	
}

ReverbParams EffectRack::getReverbParams()
{
	return reverb->getParams();
}

void EffectRack::setReverbParams(ReverbParams params)
{
	reverb->setParams(params);
}

AdsrParams EffectRack::getAdsrParams()
{
	return adsr->getParams();
}

void EffectRack::setAdsrParams(AdsrParams params)
{
	adsr->setParams(params);
}

void EffectRack::setSingleAdsrParam(AdsrParam param, float val)
{
	adsr->setSingleParam(param, val);
}


void EffectRack::setSingeReverbParam(ReverbParam param, float val)
{
	reverb->setSingleParam(param, val);
}
//===============================================================================

LayerEffectRack::LayerEffectRack(/*const double sampleRate */)
{
	effectRack = new EffectRack(48000);
}

LayerEffectRack::~LayerEffectRack()
{
	delete effectRack;
}

void LayerEffectRack::applyEffects(AudioBuffer<float> &buffer, int startSample, int numSamples)
{
	effectRack->applyEffects(buffer);
}


