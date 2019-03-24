
#pragma once
#include "../JuceLibraryCode/JuceHeader.h"
#include <map>
#include <unordered_set>
#include "StartStopBroadcaster.h"
#include "ADSR.h"

enum TupleValues
{
    currentValue = 0,
    minimalValue,
    maximalValue
};

class rmpEffect
{
public:
    rmpEffect(String _name) { name = _name; addParam("turnedOn", 1, 0, 1); };
	~rmpEffect() = default;

    class Listener {
    public:
        virtual ~Listener() = default;
        virtual	void EffectParamsChanged(rmpEffect &effect) = 0;
    };

    void addListener(Listener *listener) 
    {
        listeners.insert(listener);
    };
    void sentToListeners() 
    {
        for (Listener *listener : listeners)
            listener->EffectParamsChanged(*this);
    }
    void clearListeners() 
    {
        listeners.clear();
    };

    typedef std::tuple<float, float, float> valueTuple;
    typedef std::map<String, valueTuple> Parameters;
    
    virtual void setParams(Parameters parameters)
    {
        params = parameters;
        syncParams();
        sentToListeners();
    };
    virtual void setSingleParam(String param, float val)
    {
        std::get<TupleValues::currentValue>(params[param]) = val;
        syncParams();
        sentToListeners();
    };
    Parameters getParams()
    {
        return params;
    };
    float getParamValue(String _name)
    {
        return std::get<TupleValues::currentValue>(params[_name]);
    };
    valueTuple *getLinkToParam(String _name)
    {
        return &(params[_name]);
    };


    virtual void applyOn(AudioBuffer<float> &buffer, int startSample = 0, int numSamples = -1) = 0;
	
	String getName() { return name; };

	void turnOn() {  std::get<TupleValues::currentValue>(params["turnedOn"]) = 1; };
	void turnOff() { std::get<TupleValues::currentValue>(params["turnedOn"]) = 0; };

protected:
    void addParam(String _name, float curVal, float minVal, float maxVal)
    {
        params.emplace(_name, valueTuple(curVal, minVal, maxVal));
    };
    virtual void syncParams() = 0;
	String name;
    Parameters params;
    std::unordered_set<Listener *> listeners;
};

class rmpReverb : public rmpEffect 
{
public:
	rmpReverb(String _name, const double sampleRate = 48000.0f) : rmpEffect(_name)
    {
        reverb.setSampleRate(sampleRate);
        addParam("dryWet", 0.5, 0, 1);
        addParam("roomSize", 0.5, 0, 1);
        addParam("width", 0.5, 0, 1);
    };
	~rmpReverb() = default;
	
	void applyOn(AudioBuffer<float> &buffer, int startSample = 0, int numSamples = -1) override;

protected:
    void syncParams()
    {
        Reverb::Parameters rparams;
        rparams.dryLevel = 1.0f - getParamValue("dryWet");
        rparams.wetLevel = getParamValue("dryWet");
        rparams.roomSize = getParamValue("roomSize");
        rparams.width = getParamValue("width");
        reverb.setParameters(rparams);
    };

	Reverb reverb; 
};

class rmpADSR : public rmpEffect, public StartStopBroadcaster::Listener {
public:
    rmpADSR(String _name, const double sampleRate = 48000.0f) : rmpEffect(_name)
    { 
        addParam("attack", 0.1f, 0.0f, 1.0f);
        addParam("decay", 0.5f, 0.0f, 1.0f);
        addParam("sustain", 0.5f, 0.0f, 1.0f);
        addParam("release", 1.0f, 0.0f, 1.0f);
    };
	~rmpADSR() = default;

    void bcStarted(StartStopBroadcaster &bc) override
    {
        adsr.noteOn();
    };
    bool bcFinishing(StartStopBroadcaster &bc) override
    {
        if (adsr.isActive())
        {
            adsr.noteOff();
            locked = &bc;
            return false;
        }
        else
            return true;
    };
    void delayedFinish()
    {
        locked->reactOnDelayedStop();
    }
	void applyOn(AudioBuffer<float> &buffer, int startSample = 0, int numSamples = -1) override;

protected:
    void syncParams()
    {
        _ADSR::Parameters rparams;
        rparams.attack = getParamValue("attack");
        rparams.decay = getParamValue("decay");
        rparams.sustain = getParamValue("sustain");
        rparams.release = getParamValue("release");
        adsr.setParameters(rparams);
    };
    StartStopBroadcaster *locked;
    bool prevBufferStatus;
    _ADSR adsr;
};

class rmpVolume : public rmpEffect {
public:
    rmpVolume(String _name, const double) : rmpEffect(_name)
    {
        addParam("value", 1.0, 0, 1);
    };
    ~rmpVolume() = default;

    void applyOn(AudioBuffer<float> &buffer, int startSample = 0, int numSamples = -1);

protected:
    void syncParams()
    {
    };
};

class rmpPan : public rmpEffect {
public:
    rmpPan(String _name, const double ) : rmpEffect(_name)
    {
        addParam("value", 0, -1, 1);
    };
    ~rmpPan() = default;

    void applyOn(AudioBuffer<float> &buffer, int startSample, int numSamples);

protected:
    void syncParams()
    {
    };
};

class rmpMirrorController : public rmpEffect, public rmpEffect::Listener
{
public:
    rmpMirrorController(String _name, rmpEffect &effect, const double) : rmpEffect(_name)
    {
        params = effect.getParams();
        addParam("broken", 0, 0, 1);
    };
    ~rmpMirrorController() = default;

    void linkRack(std::shared_ptr<rmpEffect> rack)
    {
        linkedEffects.push_back(rack);
    }
    void setSingleParam(String param, float val) override
    {
        rmpEffect::setSingleParam(param, val);
        if (param != "broken")
        {
            for (auto it = linkedEffects.begin(); it != linkedEffects.end(); ++it)
                (*it)->setSingleParam(param, val);
        }
    }
    void EffectParamsChanged(rmpEffect &effect) 
    {
        if (params.size() != 1)
            setSingleParam("broken", 1);
    };

    void applyOn(AudioBuffer<float> &buffer, int startSample, int numSamples) {};

protected:
    void syncParams()
    {
    };
    std::list<std::shared_ptr<rmpEffect>> linkedEffects;
};

class rmpEffectRack : public rmpEffect
{
public:
    rmpEffectRack() : rmpEffect("") { };
    ~rmpEffectRack() = default;

    void addEffect(String _name, std::shared_ptr<rmpEffect> effect)
    {
        rack_list.emplace(_name, effect);
    };
    int getRackSize()
    {
        return rack_list.size();
    }
    void removeEffect(String _name)
    {
        rack_list.erase(_name);
    };
    
    void applyOn(AudioBuffer<float> &buffer, int startSample = 0, int numSamples = -1)
    {
        for (auto effect = rack_list.begin(); effect != rack_list.end(); ++effect)
            effect->second->applyOn(buffer, startSample, numSamples);
    };

    rmpEffect *findEffect(String nameSubstring)
    {
        for (auto effect = rack_list.begin(); effect != rack_list.end(); ++effect)
            if (effect->first.contains(nameSubstring))
                return effect->second.get();
        return nullptr;
    }

    Parameters getEffectParams(String effectName)
    {
        return rack_list[effectName]->getParams();
    };
	void setEffectParam(String effectName, String paramName, float paramValue)
    {
        rack_list[effectName]->setSingleParam(paramName, paramValue);
        sentToListeners();
    };

protected:
    void syncParams() {};
    std::map<String, std::shared_ptr<rmpEffect>> rack_list;
};



