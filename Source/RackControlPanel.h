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

	//void mouseDown(const MouseEvent &event) override;


	
	bool effectisOn;
	Label lName;

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
	ImageComponent icDepth;
	ImageComponent icWidth;
	ImageComponent icDryWet;

	ImageComponent icOnOff;

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