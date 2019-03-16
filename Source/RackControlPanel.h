/*
  ==============================================================================

    RackControlPannel.h
    Created: 5 Mar 2019 2:42:35am
    Author:  CodyDog

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "EffectRack.h"

//==============================================================================
class BaseControlPanel : public Component
{
public:
	BaseControlPanel();
	~BaseControlPanel();

	const float angle_max_val = 4.7123889803847f;
	const float angle_val = 2.3561944901923f;

	void addRack(EffectRack *rack);
	float calcKnobAngle(float param);
	float calcParam(float knobAngle);
	
	bool effectisOn;
	Label lName;

	Image imgKnob;
	Image imgSwitchOn;
	Image imgSwitchOff;
	Image imgEffectOn;
	Image imgEffectOff;
	EffectRack *effectRack;

	

	//JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RackControlPanel)
};

class rmpReverbPanel : public BaseControlPanel,
	public MouseListener
	                    
{
public:
	rmpReverbPanel();
	~rmpReverbPanel();
	void init();
	void mouseDown(const MouseEvent &event) override;
	void mouseDrag(const MouseEvent &event) override;

private:

	bool effectIsOn;
	ImageComponent icOnOff;
	ImageComponent icDepth;
	ImageComponent icWidth;
	ImageComponent icDryWet;

	Label lDepth;
	Label lWidth;
	Label lDryWet;

	ReverbParams params;

};

class rmpAdsrPanel : public BaseControlPanel,
	public MouseListener
{
public:
	rmpAdsrPanel();
	~ rmpAdsrPanel();

	void init();

	void mouseDown(const MouseEvent &event) override;
	void mouseDrag(const MouseEvent &event) override;
private:

	bool effectIsOn;
	ImageComponent icOnOff;

	ImageComponent icAttack;
	ImageComponent icDecay;
	ImageComponent icSustain;
	ImageComponent icRelease;

	Label lAttack;
	Label lDecay;
	Label lSustain;
	Label lRelease;

	AdsrParams params;
	
};

class rmpFunctionsPanel : public BaseControlPanel,
	public MouseListener
{
public:
	rmpFunctionsPanel();
	~rmpFunctionsPanel();
	void setEffects(rmpAdsrPanel *_adsr, rmpReverbPanel *_reverb);
	void mouseDrag(const MouseEvent &event) override;

private:
	rmpAdsrPanel *adsr;
	rmpReverbPanel *reverb;
	ImageComponent f[4];
	Label lf[4];
};



class LayerAdsrPanel : public BaseControlPanel
{
public:
	LayerAdsrPanel();
	//ADSR
	

private:
	Slider slAttack;
	Slider slDecay;
	Slider slSustain;
	Slider slRelease;

	Label lAttack;
	Label lDecay;
	Label lSustain;
	Label lRelease;

	bool effectIsOn;
	ImageComponent icOnOff;

};

class LayerReverbPanel : public BaseControlPanel
{
public:
	
	LayerReverbPanel();
	//Reverb
	

private:
	Slider slDryWet;
	Slider slWidth;
	Slider slDepth;

	Label lDryWet;
	Label lWidth;
	Label lDepth;

	bool effectIsOn;
	ImageComponent icOnOff;
};


class LayerEffectRackPanel : public Component
{
public:
	LayerEffectRackPanel();
	
	LayerAdsrPanel lapAdsr;
	LayerReverbPanel lrpReverb;
private:
	Label name;
	

};
