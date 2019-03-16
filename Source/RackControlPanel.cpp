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

	imgKnob = ImageFileFormat::loadFrom(File::getCurrentWorkingDirectory().getChildFile("knob.png"));
	
	imgEffectOn = ImageFileFormat::loadFrom(File::getCurrentWorkingDirectory().getChildFile("OnButton.png"));
	imgEffectOff = ImageFileFormat::loadFrom(File::getCurrentWorkingDirectory().getChildFile("OffButton.png"));

	setSize(300, 300);
}

BaseControlPanel::~BaseControlPanel()
{
	
}

void BaseControlPanel::addRack(EffectRack *rack)
{
	effectRack = rack;
}

float BaseControlPanel::calcKnobAngle(float param)
{
	return param * angle_max_val - angle_val;
}

float BaseControlPanel::calcParam(float knobAngle)
{
	return (knobAngle + angle_val) / angle_max_val;
}
//==================================================

rmpReverbPanel::rmpReverbPanel()
{
	params.dryWet = 0.5f;
	params.depth = 0.5f;
	params.width = 0.5f;

	float depthAngle = calcKnobAngle(params.depth);
	float widthAngle = calcKnobAngle(params.width);
	float drywetAngle = calcKnobAngle(params.dryWet);


	effectIsOn = true;
	addAndMakeVisible(icOnOff);
	icOnOff.addMouseListener(this, true);
	icOnOff.setSize(imgEffectOn.getWidth(), imgEffectOn.getHeight());

	if (effectIsOn)
	{
		icOnOff.setImage(imgEffectOn);
	}
	else
	{
		icOnOff.setImage(imgEffectOff);
	}
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

	icDepth.addMouseListener(this, true);
	icWidth.addMouseListener(this, true);
	icDryWet.addMouseListener(this, true);

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

void rmpReverbPanel::init()
{
	params = effectRack->getReverbParams();

	float depthAngle = calcKnobAngle(params.depth);
	float widthAngle = calcKnobAngle(params.width);
	float drywetAngle = calcKnobAngle(params.dryWet);

	AffineTransform transform;
	icDepth.setTransform(transform.rotated(depthAngle, 25.0f + 50.0f / 2.0f, 80.0f + 39.0f / 2.0f));
	icWidth.setTransform(transform.rotated(widthAngle, 75.0f + 50.0f / 2.0f, 80.0f + 39.0f / 2.0f));
	icDryWet.setTransform(transform.rotated(drywetAngle, 125.0f + 50.0f / 2.0f, 80.0f + 39.0f / 2.0f));
}

void rmpReverbPanel::mouseDown(const MouseEvent &event)
{
	if (event.eventComponent == &icOnOff)
	{
		effectRack->onOffEffect(rmpEffects::reverb);
		if (effectIsOn)
		{ 
			effectIsOn = false;
			icOnOff.setImage(imgEffectOff);
		}
		else
		{
			effectIsOn = true;
			icOnOff.setImage(imgEffectOn);
		}
	}
}

void rmpReverbPanel::mouseDrag(const MouseEvent &event)
{
	if (event.eventComponent == &icDepth)
	{
		float angle = -(float)event.getDistanceFromDragStartY() / 20;

		if (angle > angle_val)
		{
			angle = angle_val;
		}
		else if (angle < -angle_val)
		{
			angle = -angle_val;
		}

		params.depth = calcParam(angle);

		if (params.depth > 1.0f)
		{
			params.depth = 1.0f;
		}
		else if (params.depth < 0.0f)
		{
			params.depth = 0.0f;
		}

		AffineTransform transform;
		icDepth.setTransform(transform.rotated(angle, 25.0f + 50.0f / 2.0f, 80.0f + 39.0f / 2.0f));
	}
	else if (event.eventComponent == &icWidth)
	{
		float angle = -(float)event.getDistanceFromDragStartY() / 20;

		if (angle > angle_val)
		{
			angle = angle_val;
		}
		else if (angle < -angle_val)
		{
			angle = -angle_val;
		}

		params.width = calcParam(angle);

		if (params.width > 1.0f)
		{
			params.width = 1.0f;
		}
		else if (params.width < 0.0f)
		{
			params.width = 0.0f;
		}

		AffineTransform transform;
		icWidth.setTransform(transform.rotated(angle, 75.0f + 50.0f / 2.0f, 80.0f + 39.0f / 2.0f));
	}
	else if (event.eventComponent == &icDryWet)
	{
		float angle = -(float)event.getDistanceFromDragStartY() / 20;

		if (angle > angle_val)
		{
			angle = angle_val;
		}
		else if (angle < -angle_val)
		{
			angle = -angle_val;
		}

		params.dryWet = calcParam(angle);

		if (params.dryWet > 1.0f)
		{
			params.dryWet = 1.0f;
		}
		else if (params.dryWet < 0.0f)
		{
			params.dryWet = 0.0f;
		}

		AffineTransform transform;
		icDryWet.setTransform(transform.rotated(angle, 125.0f + 50.0f / 2.0f, 80.0f + 39.0f / 2.0f));
	}

	effectRack->setReverbParams(params);
}
//============================================
rmpAdsrPanel::rmpAdsrPanel()
{
	effectIsOn = false;

	addAndMakeVisible(icOnOff);
	if (effectIsOn)
	{
		icOnOff.setImage(imgEffectOn);
	}
	else
	{
		icOnOff.setImage(imgEffectOff);
	}
	
	icOnOff.setBounds(getWidth() - imgEffectOn.getWidth(), 0, imgEffectOn.getWidth(), imgEffectOn.getHeight());
	icOnOff.addMouseListener(this, true);

	
	lName.setText("ADSR", dontSendNotification);
	lName.attachToComponent(&icOnOff, true);

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
	
	icAttack.addMouseListener(this, true);
	icDecay.addMouseListener(this, true);
	icSustain.addMouseListener(this, true);
	icRelease.addMouseListener(this, true);

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

void rmpAdsrPanel::init()
{
	params = effectRack->getAdsrParams();

	float attackAngle = calcKnobAngle(params.attack);
	float decayAngle = calcKnobAngle(params.decay);
	float sustainAngle = calcKnobAngle(params.sustain);
	float releaseAngle = calcKnobAngle(params.release);


	AffineTransform transform;
	icAttack.setTransform(transform.rotated(attackAngle, 50.0f / 2.0f, 80.0f + 39.0f / 2.0f));
	icDecay.setTransform(transform.rotated(decayAngle, 50 + 50.0f / 2.0f, 80.0f + 39.0f / 2.0f));
	icSustain.setTransform(transform.rotated(sustainAngle, 100 + 50.0f / 2.0f, 80.0f + 39.0f / 2.0f));
	icRelease.setTransform(transform.rotated(releaseAngle, 150 + 50.0f / 2.0f, 80.0f + 39.0f / 2.0f));
	
}

void rmpAdsrPanel::mouseDown(const MouseEvent &event)
{
	if (event.eventComponent == &icOnOff)
	{
		effectRack->onOffEffect( rmpEffects::adsr );
		if (effectIsOn)
		{
			effectIsOn = false;
			icOnOff.setImage(imgEffectOff);
		}
		else
		{
			effectIsOn = true;
			icOnOff.setImage(imgEffectOn);
		}
	}
}

void rmpAdsrPanel::mouseDrag(const MouseEvent &event)
{
	if (event.eventComponent == &icAttack)
	{
		float angle = - (float) event.getDistanceFromDragStartY() / 20;

		if (angle > angle_val)
		{
			angle = angle_val;
		}
		else if (angle < -angle_val)
		{
			angle = -angle_val;
		}

		params.attack = calcParam(angle);

		if (params.attack > 1.0f)
		{
			params.attack = 1.0f;
		}
		else if (params.attack < 0.0f)
		{
			params.attack = 0.0f;
		}

		AffineTransform transform;
		icAttack.setTransform( transform.rotated(angle, 50.0f / 2.0f, 80.0f + 39.0f / 2.0f));
	}
	else if (event.eventComponent == &icDecay)
	{
		float angle = -(float)event.getDistanceFromDragStartY() / 20;

		if (angle > angle_val)
		{
			angle = angle_val;
		}
		else if (angle < -angle_val)
		{
			angle = -angle_val;
		}

		params.decay = calcParam(angle);

		if (params.decay > 1.0f)
		{
			params.decay = 1.0f;
		}
		else if (params.decay < 0.0f)
		{
			params.decay = 0.0f;
		}

		AffineTransform transform;
		icDecay.setTransform(transform.rotated(angle, 50 + 50.0f / 2.0f, 80.0f + 39.0f / 2.0f));
	}
	else if (event.eventComponent == &icRelease)
	{
		float angle = -(float)event.getDistanceFromDragStartY() / 20;

		if (angle > angle_val)
		{
			angle = angle_val;
		}
		else if (angle < -angle_val)
		{
			angle = -angle_val;
		}

		params.release = calcParam(angle);

		if (params.release > 1.0f)
		{
			params.release = 1.0f;
		}
		else if (params.release < 0.0f)
		{
			params.release = 0.0f;
		}

		AffineTransform transform;
		icRelease.setTransform(transform.rotated(angle, 150 + 50.0f / 2.0f, 80.0f + 39.0f / 2.0f));
	}
	else if (event.eventComponent == &icSustain)
	{
		float angle = -(float)event.getDistanceFromDragStartY() / 20;

		if (angle > angle_val)
		{
			angle = angle_val;
		}
		else if (angle < -angle_val)
		{
			angle = -angle_val;
		}

		params.sustain = calcParam(angle);

		if (params.sustain > 1.0f)
		{
			params.sustain = 1.0f;
		}
		else if (params.sustain < 0.0f)
		{
			params.sustain = 0.0f;
		}

		AffineTransform transform;
		icSustain.setTransform(transform.rotated(angle, 100 + 50.0f / 2.0f, 80.0f + 39.0f / 2.0f));
	}
	effectRack->setAdsrParams(params);
}


rmpFunctionsPanel::rmpFunctionsPanel( )
{
	lName.setText("Effects", dontSendNotification);
	lName.attachToComponent(this, false);
	
	for (int i = 0; i < 4; ++i)
	{
		addAndMakeVisible(f[i]);
		f[i].addMouseListener(this, true);
		
		f[i].setImage(imgKnob);
		f[i].setBounds(i * 50, 30, 50, 39);
		lf[i].setText(juce::String("Effect ") + juce::String(i + 1), dontSendNotification);
		lf[i].attachToComponent(f + i, false);
	}
}

rmpFunctionsPanel::~rmpFunctionsPanel()
{

}

void rmpFunctionsPanel::setEffects(rmpAdsrPanel *_adsr, rmpReverbPanel *_reverb)
{
	reverb = _reverb;
	adsr = _adsr;
}

void rmpFunctionsPanel::mouseDrag(const MouseEvent &event)
{

	if (event.eventComponent == f) //Depth + Release
	{
		float angle = -(float)event.getDistanceFromDragStartY() / 20;

		if (angle > angle_val)
		{
			angle = angle_val;
		}
		else if (angle < -angle_val)
		{
			angle = -angle_val;
		}

		AffineTransform transform;
		f[0].setTransform(transform.rotated(angle, 50.0f / 2.0f, 30.0f + 39.0f / 2.0f));
		float depth_val = calcParam(angle);
		float release_val = 1 - calcParam(angle);
		
		effectRack->setSingleAdsrParam(epRelease, release_val);
		effectRack->setSingeReverbParam(epDepth, depth_val);
	} 
	else if (event.eventComponent == f + 1) //width + attack
	{
		float angle = -(float)event.getDistanceFromDragStartY() / 20;

		if (angle > angle_val)
		{
			angle = angle_val;
		}
		else if (angle < -angle_val)
		{
			angle = -angle_val;
		}

		AffineTransform transform;
		f[1].setTransform(transform.rotated(angle, 50 + 50.0f / 2.0f, 30.0f + 39.0f / 2.0f));

		float width_val = calcParam(angle);
		float attack_val = calcParam(angle);

		effectRack->setSingleAdsrParam(epAttack, attack_val);
		effectRack->setSingeReverbParam(epWidth, width_val);
	}
	else if (event.eventComponent == f + 2) //sustain + wet
	{
		float angle = -(float)event.getDistanceFromDragStartY() / 20;

		if (angle > angle_val)
		{
			angle = angle_val;
		}
		else if (angle < -angle_val)
		{
			angle = -angle_val;
		}

		AffineTransform transform;
		f[2].setTransform(transform.rotated(angle, 100 + 50.0f / 2.0f, 30.0f + 39.0f / 2.0f));

		float sustain_val = calcParam(angle);
		float drywet_val = 1 - calcParam(angle);

		effectRack->setSingleAdsrParam(epSustain, sustain_val);
		effectRack->setSingeReverbParam(epDryWet, drywet_val);
	}
	else if (event.eventComponent == f + 3) // decay + width
	{
		float angle = -(float)event.getDistanceFromDragStartY() / 20;

		if (angle > angle_val)
		{
			angle = angle_val;
		}
		else if (angle < -angle_val)
		{
			angle = -angle_val;
		}

		AffineTransform transform;
		f[3].setTransform(transform.rotated(angle, 150 + 50.0f / 2.0f, 30.0f + 39.0f / 2.0f));

		float width_val = calcParam(angle);
		float decay_val = calcParam(angle);

		effectRack->setSingleAdsrParam(epDecay, decay_val);
		effectRack->setSingeReverbParam(epWidth, width_val);
	}
	

	adsr->init();
	reverb->init();

}

LayerReverbPanel::LayerReverbPanel()
{
	lName.setText("Reverb", dontSendNotification);
	effectIsOn = true;
	if (effectIsOn)
	{
		icOnOff.setImage(imgEffectOn);
	}
	else
	{
		icOnOff.setImage(imgEffectOff);
	}

	addAndMakeVisible(icOnOff);

	addAndMakeVisible(slDryWet);
	addAndMakeVisible(slDepth);
	addAndMakeVisible(slWidth);

	slDryWet.setBounds(50, 30, 200, 30);
	slDepth.setBounds(50, 60, 200, 30);
	slWidth.setBounds(50, 90, 200, 30);
	
	slDryWet.setRange(Range<double>(0, 1), 0.01);
	slDepth.setRange(Range<double>(0, 1), 0.01);
	slWidth.setRange(Range<double>(0, 1), 0.01);


	lDryWet.setText("Dry/Wet", dontSendNotification);
	lDepth.setText("Depth", dontSendNotification);
	lWidth.setText("Width", dontSendNotification);

	lDryWet.attachToComponent(&slDryWet, true);
	lDepth.attachToComponent(&slDepth, true);
	lWidth.attachToComponent(&slWidth, true);

	icOnOff.setBounds(getWidth() - 39, 0, 39, 39);
	lName.attachToComponent(&icOnOff, true);
}

LayerAdsrPanel::LayerAdsrPanel()
{

	lName.setText("Adsr", dontSendNotification);

	effectIsOn = true;
	if (effectIsOn)
	{
		icOnOff.setImage(imgEffectOn);
	}
	else
	{
		icOnOff.setImage(imgEffectOff);
	}

	addAndMakeVisible(icOnOff);

	icOnOff.setBounds(getWidth() - 39, 0, 39, 39);
	lName.attachToComponent(&icOnOff, true);

	lAttack.setText("Attack", dontSendNotification);
	lDecay.setText("Decay", dontSendNotification);
	lSustain.setText("Sustain", dontSendNotification);
	lRelease.setText("Release", dontSendNotification);

	lAttack.attachToComponent(&slAttack, true);
	lDecay.attachToComponent(&slDecay, true);
	lSustain.attachToComponent(&slSustain, true);
	lRelease.attachToComponent(&slRelease, true);

	addAndMakeVisible(slAttack);
	addAndMakeVisible(slDecay);
	addAndMakeVisible(slSustain);
	addAndMakeVisible(slRelease);

	slAttack.setRange(Range<double>(0, 1), 0.01);
	slDecay.setRange(Range<double>(0, 1), 0.01);
	slSustain.setRange(Range<double>(0, 1), 0.01);
	slRelease.setRange(Range<double>(0, 1), 0.01);

	slAttack.setBounds(50, 30, 200, 30);
	slDecay.setBounds(50, 60, 200, 30);
	slSustain.setBounds(50, 90, 200, 30);
	slRelease.setBounds(50, 120, 200, 30);

}


LayerEffectRackPanel::LayerEffectRackPanel()
{
	name.setText("Layer Effect Rack", dontSendNotification);
	name.attachToComponent(this, false);
	setSize(120, 300);
	addAndMakeVisible( lapAdsr );
	addAndMakeVisible( lrpReverb );

	lapAdsr.setBounds(0, 0, 300, 150);
	lrpReverb.setBounds(0, 170, 300, 150);
}
