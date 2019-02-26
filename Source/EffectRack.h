/*
  ==============================================================================

    EffectRack.h
    Created: 23 Feb 2019 5:44:28pm
    Author:  CodyDog

  ==============================================================================
*/

#pragma once
#include "../JuceLibraryCode/JuceHeader.h"

class rmpEffect
{
    public:
        rmpEffect();
        ~rmpEffect();
    
       virtual void applyEffect(AudioBuffer<float> &buffer);
    private:
        juce::String id;
};

class rmpReverb : public rmpEffect
{
    public:
	    rmpReverb();
	    ~rmpReverb();

	    void applyEffect(AudioBuffer<float> &buffer) override;
private:

	Reverb reverb;
};

class rmpADSR : public rmpEffect
{
    public:
	    rmpADSR();
	    ~rmpADSR();

	    void applyEffect(AudioBuffer<float> &buffer) override;
private:
	//ADSR adsr;
	int startSample;
	int endSample;
};



class EffectRack
{
    public:
        //==================================================
        EffectRack();
        ~EffectRack();
        
        //==================================================
        void applyEffects(AudioBuffer<float> &buffer);
        void onEffect(juce::String id);
        void offEffect(juce::String id);
        
    private:
		rmpReverb reverb;
		rmpADSR adsr;
       std::vector<rmpEffect> effectRack;
	   std::vector<rmpEffect>::iterator ptr;
};