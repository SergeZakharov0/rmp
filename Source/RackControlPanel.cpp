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

	setSize(200, 150);
}

BaseControlPanel::~BaseControlPanel()
{
	
}

//==================================================

rmpReverbPanel::rmpReverbPanel()
{
	
	addAndMakeVisible(icOnOff);
	icOnOff.setSize(imgEffectOn.getWidth(), imgEffectOn.getHeight());
	icOnOff.setImage( imgEffectOn );
	icOnOff.setBounds( getWidth() - imgEffectOn.getWidth(), 0, icOnOff.getWidth(), icOnOff.getHeight());

	lName.setText("Reverb", dontSendNotification);
	lName.attachToComponent(&icOnOff, true);

	icDepth.setImage(imgKnob);
	icWidth.setImage(imgKnob);
	icDryWet.setImage(imgKnob);

	addAndMakeVisible(icDepth);
	addAndMakeVisible(icWidth);
	addAndMakeVisible(icDryWet);

	icDepth.setBounds(25, 80, 50, 39);
	icWidth.setBounds(75, 80, 50, 39);
	icDryWet.setBounds(125, 80, 50, 39);

	lDepth.setText("Depth", dontSendNotification);
	lWidth.setText("Width", dontSendNotification);
	lDryWet.setText("Dry/Wet", dontSendNotification);

	lDepth.attachToComponent(&icDepth, false);
	lWidth.attachToComponent(&icWidth, false);
	lDryWet.attachToComponent(&icDryWet, false);

}
rmpReverbPanel::~rmpReverbPanel()
{

}
//============================================
rmpAdsrPanel::rmpAdsrPanel()
{
	addAndMakeVisible(icOnOff);
	icOnOff.setImage(imgEffectOn);
	icOnOff.setBounds(getWidth() - imgEffectOn.getWidth(), 0, imgEffectOn.getWidth(), imgEffectOn.getHeight());
	
	lName.setText("ADSR", dontSendNotification);
	lName.attachToComponent(&icOnOff, true);
	//lName.setTopLeftPosition(50, 0);

	addAndMakeVisible(icAttack);
	addAndMakeVisible(icDecay);
	addAndMakeVisible(icSustain);
	addAndMakeVisible(icRelease);

	icAttack.setImage(imgKnob);
	icDecay.setImage(imgKnob);
	icSustain.setImage(imgKnob);
	icRelease.setImage(imgKnob);

	icAttack.setBounds( 0, 80, 50, 39 );
	icDecay.setBounds(50, 80, 50, 39);
	icSustain.setBounds(100, 80, 50, 39);
	icRelease.setBounds(150, 80, 50, 39);

	lAttack.setText("Attack", dontSendNotification);
	lDecay.setText("Decay", dontSendNotification);
	lSustain.setText("Sustain", dontSendNotification);
	lRelease.setText("Release", dontSendNotification);

	lAttack.attachToComponent(&icAttack, false);
	lDecay.attachToComponent(&icDecay, false);
	lSustain.attachToComponent(&icSustain, false);
	lRelease.attachToComponent(&icRelease, false);
}

rmpAdsrPanel::~rmpAdsrPanel()
{

}



