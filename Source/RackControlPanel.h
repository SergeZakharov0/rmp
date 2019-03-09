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
