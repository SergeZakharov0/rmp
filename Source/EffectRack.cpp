
#include "EffectRack.h"

void rmpReverb::applyOn(AudioBuffer<float> &buffer, LockingVoice *, int startSample, int numSamples)
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

void rmpADSR::applyOn(AudioBuffer<float> &buffer, LockingVoice *caller, int startSample, int numSamples)
{
    if (std::get<TupleValues::currentValue>(params["turnedOn"]) == 0)
        return;

    if (prevBufferStatuses[caller] == true && !adsrs[caller].isActive())
        delayedRelease(caller);
    prevBufferStatuses[caller] = adsrs[caller].isActive();

    adsrs[caller].applyEnvelopeToBuffer(buffer, startSample, numSamples);
}

void rmpEffectRack::parseConfig(XmlElement *config, std::list<LockingVoice *> &voices)
{
    forEachXmlChildElement(*config, effect_item)
    {
        if (effect_item->hasTagName("volume"))
        {
            String _name = "volume" + String(rack_list.size() + 1);
            rmpVolume *eff = new rmpVolume(_name, 48000);
            eff->setSingleParam("value", effect_item->getChildByName("value")->getAllSubText().getFloatValue());
            addEffect(_name, eff);
        }
        if (effect_item->hasTagName("pan"))
        {
            String _name = "pan" + String(rack_list.size() + 1);
            rmpPan *eff = new rmpPan(_name, 48000);
            eff->setSingleParam("value", effect_item->getChildByName("value")->getAllSubText().getFloatValue());
            addEffect(_name, eff);
        }
        if (effect_item->hasTagName("reverb"))
        {
            String _name = "reverb" + String(rack_list.size() + 1);
            rmpReverb *eff = new rmpReverb(_name);
            eff->setSingleParam("dryWet", effect_item->getChildByName("dryWet")->getAllSubText().getFloatValue());
            eff->setSingleParam("roomSize", effect_item->getChildByName("roomSize")->getAllSubText().getFloatValue());
            eff->setSingleParam("width", effect_item->getChildByName("width")->getAllSubText().getFloatValue());
            addEffect(_name, eff);
        }
        if (effect_item->hasTagName("adsr"))
        {
            String _name = "adsr" + String(rack_list.size() + 1);
            rmpADSR *eff = new rmpADSR(_name, voices);
            eff->setSingleParam("attack", effect_item->getChildByName("attack")->getAllSubText().getFloatValue());
            eff->setSingleParam("decay", effect_item->getChildByName("decay")->getAllSubText().getFloatValue());
            eff->setSingleParam("sustain", effect_item->getChildByName("sustain")->getAllSubText().getFloatValue());
            eff->setSingleParam("release", effect_item->getChildByName("release")->getAllSubText().getFloatValue());
            addEffect(_name, eff);
            for (std::list<LockingVoice *>::iterator it = voices.begin(); it != voices.end(); ++it)
                (*it)->addLockingListener(eff);
        }
    }
}

void rmpVolume::applyOn(AudioBuffer<float> &buffer, LockingVoice *, int startSample, int numSamples)
{
    buffer.applyGain(startSample, numSamples, getParamValue("value"));
}

void rmpPan::applyOn(AudioBuffer<float> &buffer, LockingVoice *, int startSample, int numSamples)
{
    buffer.applyGain(0, startSample, numSamples, 1 - getParamValue("value"));
    buffer.applyGain(1, startSample, numSamples, getParamValue("value"));
}
