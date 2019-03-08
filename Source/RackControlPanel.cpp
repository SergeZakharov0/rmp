/*
  ==============================================================================

    RackControlPannel.cpp
    Created: 5 Mar 2019 2:42:35am
    Author:  CodyDog

  ==============================================================================
*/

#include "../JuceLibraryCode/JuceHeader.h"
#include "RackControlPanel.h"

//==============================================================================

BaseControlPanel::BaseControlPanel()
{
	imgKnob = ImageFileFormat::loadFrom(File("C:/Projects/rmp/knob.png"));
	imgEffectOn = ImageFileFormat::loadFrom(File("C:/Projects/rmp/OnButton.png"));
	imgEffectOff = ImageFileFormat::loadFrom(File("C:/Projects/rmp/OffButton.png"));

	setSize(150, 80);
}

BaseControlPanel::~BaseControlPanel()
{
	
}


//==================================================

rmpReverbPanel::rmpReverbPanel()
{
	setSize(150, 100);
	
	addAndMakeVisible(icOnOff);
	icOnOff.setSize(imgEffectOn.getWidth(), imgEffectOn.getHeight());
	icOnOff.setImage( imgEffectOn );
	icOnOff.setBounds(0, 0, icOnOff.getWidth(), icOnOff.getHeight());

	lName.setText("Reverb", dontSendNotification);
	lName.attachToComponent(this, true);

	icDepth.setImage(imgKnob);
	icWidth.setImage(imgKnob);
	icDryWet.setImage(imgKnob);

	addAndMakeVisible(icDepth);
	addAndMakeVisible(icWidth);
	addAndMakeVisible(icDryWet);

	icDepth.setBounds(0, 40, 39, 39);
	icWidth.setBounds(40, 40, 39, 39);
	icDryWet.setBounds(80, 40, 39, 39);

}
rmpReverbPanel::~rmpReverbPanel()
{

}
//============================================
rmpAdsrPanel::rmpAdsrPanel()
{
	setSize(200, 100);
	addAndMakeVisible(icOnOff);
	icOnOff.setImage(imgEffectOn);
	icOnOff.setBounds(0, 0, imgEffectOn.getWidth(), imgEffectOn.getHeight());
	
	lName.setText("ADSR", dontSendNotification);
	lName.attachToComponent(this, true);
	lName.setTopLeftPosition(50, 0);

	addAndMakeVisible(icAttack);
	addAndMakeVisible(icDecay);
	addAndMakeVisible(icSustain);
	addAndMakeVisible(icRelease);

	icAttack.setImage(imgKnob);
	icDecay.setImage(imgKnob);
	icSustain.setImage(imgKnob);
	icRelease.setImage(imgKnob);

	icAttack.setBounds( 0, 50, 39, 39 );
	icDecay.setBounds(40, 50, 39, 39);
	icSustain.setBounds(80, 50, 39, 39);
	icRelease.setBounds(120, 50, 39, 39);
}

rmpAdsrPanel::~rmpAdsrPanel()
{

}



