/*
  ==============================================================================

    EffectRack.h
    Created: 23 Feb 2019 5:44:28pm
    Author:  CodyDog

  ==============================================================================
*/

#pragma once
#include "../JuceLibraryCode/JuceHeader.h"
#include "EffectRackConfig.h"

class rmpEffect
{
    public:
        rmpEffect();
        ~rmpEffect();
    
       virtual void applyEffect(AudioBuffer<float> &buffer);

};

class rmpReverb : public rmpEffect
{
    public:
	    rmpReverb(const double sampleRate);
	    ~rmpReverb();

		void setParams();
		void setParams( ReverbParams parameters);
		ReverbParams getParams();
	    void applyEffect(AudioBuffer<float> &buffer) override;
private:
	void setSampleRate(const double sampleRate);
	ReverbParams params;

	Reverb reverb;
};

class rmpADSR : public rmpEffect
{
    public:
	    rmpADSR();
	    ~rmpADSR();

		AdsrParams getParams();
		void setParams();
		void setParams(AdsrParams parameters);

	    void applyEffect(AudioBuffer<float> &buffer) override;
private:
	AdsrParams params;
	ADSR adsr;
	int startSample;
	int endSample;
};



class EffectRack : public Slider::Listener,
	               public MouseListener
{
    public:
        //==================================================
        EffectRack(const double sampleRate);
        ~EffectRack();
        
        //==================================================
        void applyEffects(AudioBuffer<float> &buffer);
		void sliderValueChanged(Slider* slider) override;
		void mouseDown(const MouseEvent &event) override;

        void onOffEffect(int id);

		ReverbParams getReverbParams();
		void setReverbParams(ReverbParams params);

		AdsrParams getAdsrParams();
		void setAdsrParams(AdsrParams params);
        
    private:
		void applyVolToBuffer(AudioBuffer<float>& buffer);
		void volValChanged(float val);
		void panValChanged(float val);
		
		float volume;
		float pan;
		rmpReverb *reverb;
		rmpADSR *adsr;
		bool effectIsOff[rmpEffects::commonEffectsVal];
       std::vector<rmpEffect> effectRack;
	   std::vector<rmpEffect>::iterator ptr;
};

class LayerEffectRack
{
public:
	LayerEffectRack();
	~LayerEffectRack();

	void applyEffects(AudioBuffer<float> &buffer, int startSample, int numSamples);
};




