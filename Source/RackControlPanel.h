/*
  ==============================================================================

    RackControlPannel.h
    Created: 5 Mar 2019 2:42:35am
    Author:  CodyDog

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

//==============================================================================
class BaseControlPanel : public Component,
	public MouseListener
{
public:
	BaseControlPanel();
	~BaseControlPanel();
	
	bool effectisOn;
	Label lName;
	//ImageComponent icOnOff;

	Image imgKnob;
	Image imgSwitchOn;
	Image imgSwitchOff;
	Image imgEffectOn;
	Image imgEffectOff;
	

	//JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(RackControlPanel)
};

class rmpReverbPanel : public BaseControlPanel
{
public:
	rmpReverbPanel();
	~rmpReverbPanel();

private:
	ImageComponent icOnOff;
	ImageComponent icDepth;
	ImageComponent icWidth;
	ImageComponent icDryWet;

	Label lDepth;
	Label lWidth;
	Label lDryWet;

};

class rmpAdsrPanel : public BaseControlPanel
{
public:
	rmpAdsrPanel();
	~ rmpAdsrPanel();

private:
	ImageComponent icOnOff;

	ImageComponent icAttack;
	ImageComponent icDecay;
	ImageComponent icSustain;
	ImageComponent icRelease;

	Label lAttack;
	Label lDecay;
	Label lSustain;
	Label lRelease;

};

class RackControlPanel : public Component
{
public:
	RackControlPanel();
	~RackControlPanel();

private:
	rmpReverbPanel reverb;
	rmpAdsrPanel adsr;
};