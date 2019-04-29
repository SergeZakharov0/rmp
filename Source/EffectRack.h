
#pragma once
#include "../JuceLibraryCode/JuceHeader.h"
#include <map>
#include <unordered_set>
#include "StartStopBroadcaster.h"
#include "ADSR.h"
#include "MVerb.h"

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
        float prevValue = std::get<TupleValues::currentValue>(params[param]);
        std::get<TupleValues::currentValue>(params[param]) = val;
        syncParams();
        if (prevValue != val)
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

		mreverb.setSampleRate(sampleRate);
		mreverb.setParameter(MVerb<float>::DAMPINGFREQ, 0.0028);
		mreverb.setParameter(MVerb<float>::DENSITY, 0.002);
		mreverb.setParameter(MVerb<float>::BANDWIDTHFREQ, 0.5);
		mreverb.setParameter(MVerb<float>::DECAY, 0.5);
		mreverb.setParameter(MVerb<float>::PREDELAY, 0.5);
		mreverb.setParameter(MVerb<float>::GAIN, 1.);
		mreverb.setParameter(MVerb<float>::MIX, 0.5);
		mreverb.setParameter(MVerb<float>::EARLYMIX, 0.5);
		mreverb.setParameter(MVerb<float>::SIZE, 0.75);

    };
	~rmpReverb() = default;
	
	void applyOn(AudioBuffer<float> &buffer, int startSample = 0, int numSamples = -1) override;

protected:
    void syncParams()
    {
		mreverb.setParameter(MVerb<float>::MIX, getParamValue("dryWet"));
		mreverb.setParameter(MVerb<float>::DENSITY, 1.0 - getParamValue("width") );
		mreverb.setParameter(MVerb<float>::DECAY, getParamValue("roomSize"));
 
    };
	MVerb<float> mreverb;
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
        if (adsr.isActive() && std::get<TupleValues::currentValue>(params["turnedOn"]) != 0)
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
        locked = 0;
    }
    void setSingleParam(String param, float val)
    {
        rmpEffect::setSingleParam(param, val);
        if (param == "turnedOn" && val == 0.0 && locked)
            delayedFinish();
    };

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
    StartStopBroadcaster *locked = 0;
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

class rmpDelay : public rmpEffect
{
public:
    rmpDelay(String _name, const double _sampleRate) : rmpEffect(_name)
    {
        addParam("dryWet", 1, 0, 1);
        addParam("time", 0.5, 0, 1);
        addParam("feedback", 0.5, 0, 1);

        sampleRate = _sampleRate;
        bufferSize = 2 * sampleRate;
        d_start_l = new float[bufferSize];
        d_start_r = new float[bufferSize];
        d_end_l = d_start_l + bufferSize;
        d_end_r = d_start_r + bufferSize;

        for (int iter = 0; iter < bufferSize; ++iter)
        {
            d_start_l[iter] = 0;
            d_start_r[iter] = 0;
        }

        timeParam = &std::get<TupleValues::currentValue>(params["time"]);
        dryWetParam = &std::get<TupleValues::currentValue>(params["dryWet"]);
        feedbackParam = &std::get<TupleValues::currentValue>(params["feedback"]);

        write_l = d_start_l;
        write_r = d_start_r;
        read_l = d_start_l + ((write_l - d_start_l) + ((int)(*timeParam * sampleRate))) % bufferSize;
        read_r = d_start_r + ((write_r - d_start_r) + ((int)(*timeParam * sampleRate))) % bufferSize;
    };
    ~rmpDelay()
    {
        delete(d_start_l);
        delete(d_start_r);
    }

    void applyOn(AudioBuffer<float> &buffer, int startSample, int numSamples)
    {
        if (std::get<TupleValues::currentValue>(params["turnedOn"]) == 0)
            return;
        float *c_start_l = buffer.getWritePointer(0) + startSample;
        float *c_start_r = buffer.getWritePointer(1) + startSample;
       
        for (int iter = 0; iter < numSamples; ++iter)
        {
            *(c_start_l) += *(read_l) * *dryWetParam * *feedbackParam;
            *(c_start_r) += *(read_r) * *dryWetParam * *feedbackParam;
            *(write_l) = *(c_start_l);
            *(write_r) = *(c_start_r);


            ++read_l; ++read_r;
            if (read_l >= d_end_l)
                read_l = d_start_l;
            if (read_r >= d_end_r)
                read_r = d_start_r;

            write_l = d_start_l + ((read_l - d_start_l) + ((int)(*timeParam * sampleRate))) % bufferSize;
            write_r = d_start_r + ((read_r - d_start_r) + ((int)(*timeParam * sampleRate))) % bufferSize;
            float a = (write_l - read_l)/sampleRate;
            int b = read_l - d_start_l;
            ++c_start_l; ++c_start_r;
        }
    };
protected:
    float sampleRate;
    
    float *d_start_l, *d_start_r;
    float *d_end_l, *d_end_r;
    int bufferSize;
    float *timeParam, *dryWetParam, *feedbackParam;

    float *read_l, *read_r, *write_l, *write_r;

    void syncParams()
    {

    };
};

class rmpFunctionalController : public rmpEffect, public rmpEffect::Listener
{
public:
    rmpFunctionalController(String _name, const double) : rmpEffect(_name)
    {
        addParam("value", 0, 0, 1);
    };
    ~rmpFunctionalController()
    {
        for (auto it = linked.begin(); it != linked.end(); ++it)
            delete(std::get<2>(*it));
    }

    class Law
    {
    public:
        Law(float _minValue = 0, float _maxValue = 1) { minValue = _minValue; maxValue = _maxValue; };
        ~Law() = default;
        virtual float applyLaw(float inp) { return inp * (maxValue - minValue) + minValue; }
    protected:
        float minValue, maxValue;
    };

    class inverseLaw : public Law
    {
    public:
        inverseLaw(float _minValue = 0, float _maxValue = 1) { minValue = _minValue; maxValue = _maxValue; };
        ~inverseLaw() = default;
        virtual float applyLaw(float inp) { return maxValue - inp * (maxValue - minValue); }
    };
    typedef std::tuple <String, rmpEffect *, rmpFunctionalController::Law *> Parameter;

    void link(Parameter param)
    {
        linked.push_back(param);
        std::get<1>(param)->setSingleParam(std::get<0>(param), std::get<2>(param)->applyLaw(getParamValue("value")));
        std::get<1>(param)->addListener(this);
    }
    void setSingleParam(String param, float val) override
    {
        rmpEffect::setSingleParam(param, val);
        if (getParamValue("turnedOn") != 0)
        {
            changedOnce = true;
            for (auto it = linked.begin(); it != linked.end(); ++it)
                std::get<1>(*it)->setSingleParam(std::get<0>(*it), std::get<2>(*it)->applyLaw(getParamValue("value")));
            changedOnce = false;
        }
    }
    void EffectParamsChanged(rmpEffect &effect)
    {
    };

    void applyOn(AudioBuffer<float> &buffer, int startSample, int numSamples) {};

protected:
    void syncParams()
    {
    };
    std::list<Parameter> linked;
    bool changedOnce = false;
};

class rmpMirrorController : public rmpEffect, public rmpEffect::Listener
{
public:
    rmpMirrorController(String _name, rmpEffect &effect, const double) : rmpEffect(_name)
    {
        params = effect.getParams();
    };
    ~rmpMirrorController() = default;

    void linkRack(std::shared_ptr<rmpEffect> rack)
    {
        linkedEffects.push_back(rack);
    }
    void setSingleParam(String param, float val) override
    {
        rmpEffect::setSingleParam(param, val);
        for (auto it = linkedEffects.begin(); it != linkedEffects.end(); ++it)
            (*it)->setSingleParam(param, val);
    }
    void EffectParamsChanged(rmpEffect &effect) 
    {
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



