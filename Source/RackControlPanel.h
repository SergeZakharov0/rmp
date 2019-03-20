
#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

#include "EffectRack.h"
#include <map>


class globalEffectRackLookAndFeel : public LookAndFeel_V4 {
public:
    void drawLinearSlider(Graphics &g, int x, int y, int width, int height, float, float, float, const Slider::SliderStyle, Slider &slider) {
        Image bg = ImageCache::getFromMemory(BinaryData::librarysliderback_png, BinaryData::librarysliderback_pngSize);
        g.drawImage(bg, x + width / 4, y, width / 2, height + y, 0, 0, bg.getWidth(), bg.getHeight());
        Image head = ImageCache::getFromMemory(BinaryData::librarysliderhead_png, BinaryData::librarysliderhead_pngSize);
        float percentPos = (float)slider.getValue();
        g.drawImage(head, x, (int)(y + (1 - percentPos) * (height - head.getHeight() / 2)), width, width, 0, 0, head.getWidth(), head.getHeight());
    };

    void drawRotarySlider(Graphics& g, int, int, int width, int, float sliderPos,
        const float rotaryStartAngle, const float rotaryEndAngle, Slider&) override
    {
        Image im = ImageCache::getFromMemory(BinaryData::knob_png, BinaryData::knob_pngSize);

        AffineTransform transform = AffineTransform().rotated((rotaryEndAngle - rotaryStartAngle) * sliderPos + rotaryStartAngle, (float)im.getWidth() / 2, (float)im.getHeight() / 2).scaled((float)width / (float)im.getWidth());
        g.drawImageTransformed(im, transform);
    }

};


class rmpButton : public Button 
{
public:
    rmpButton() : Button("") {};
protected:
    void paintButton(Graphics &, bool, bool) {};
};

class EffectControlPanel : public Component, public rmpEffect::Listener, public Button::Listener, public Slider::Listener
{
public:
    EffectControlPanel()
    {
        setLookAndFeel(&lookAndFeel);
    }
    ~EffectControlPanel() 
    {
        setLookAndFeel(nullptr);
        for (std::map<Component *, rmpEffect::valueTuple *>::iterator it = links.begin(); it != links.end(); ++it)
            delete(it->first);
    }

    void addRotarySlider(rmpEffect::valueTuple *param, Rectangle<int> bounds, String name, Image)
    {
        Slider *slider = new Slider(Slider::SliderStyle::RotaryHorizontalVerticalDrag, Slider::TextEntryBoxPosition::NoTextBox);
        slider->setName(name);
        slider->setBounds(bounds);
        addAndMakeVisible(*slider);
        links.emplace(slider, param);
        slider->addListener(this);
        if (param)
            slider->setMinAndMaxValues(std::get<TupleValues::minimalValue>(*param), std::get<TupleValues::maximalValue>(*param));
    }
    void addHorizontalSlider(rmpEffect::valueTuple *param, Rectangle<int> bounds, String name, Image)
    {
        Slider *slider = new Slider(Slider::SliderStyle::LinearHorizontal, Slider::TextEntryBoxPosition::NoTextBox);
        slider->setName(name);
        slider->setBounds(bounds);
        addAndMakeVisible(*slider);
        links.emplace(slider, param);
        slider->addListener(this);
        if (param)
            slider->setMinAndMaxValues(std::get<TupleValues::minimalValue>(*param), std::get<TupleValues::maximalValue>(*param));
    }
    void addButton(rmpEffect::valueTuple *param, Rectangle<int> bounds, String name, Image)
    {
        Button *button = new rmpButton();
        button->setName(name);
        button->setBounds(bounds);
        addAndMakeVisible(*button);
        links.emplace(button, param);
        button->addListener(this);
        if (param)
            button->setToggleState(std::get<TupleValues::currentValue>(*param) != 0, dontSendNotification);
    }
    void setLink(rmpEffect::valueTuple *param, String _componentName)
    {
        for (std::map<Component *, rmpEffect::valueTuple *>::iterator it = links.begin(); it != links.end(); ++it)
            if (it->first->getName() == _componentName)
            {
                it->second = param;

                if (param)
                {
                    Slider *ptr_slider = dynamic_cast<Slider *>(it->first);
                    if (ptr_slider)
                    {
                        if (ptr_slider->getSliderStyle() == Slider::SliderStyle::LinearHorizontal)
                            ptr_slider->setMinAndMaxValues(std::get<TupleValues::minimalValue>(*param), std::get<TupleValues::maximalValue>(*param));
                        ptr_slider->setValue(std::get<TupleValues::currentValue>(*param)*10);
                    }

                    Button *ptr_button = dynamic_cast<Button *>(it->first);
                    if (ptr_button)
                    {
                        ptr_button->setToggleState(std::get<TupleValues::currentValue>(*it->second) != 0, dontSendNotification);
                    }
                }
            }
    }
    void EffectParamsChanged(rmpEffect *) 
    {
        for (std::map<Component *,rmpEffect::valueTuple *>::iterator it = links.begin(); it != links.end(); ++it)
        {
            Slider *ptr_slider = dynamic_cast<Slider *>(it->first);
            if (ptr_slider != nullptr && it->second != nullptr)
                if (std::get<TupleValues::currentValue>(*it->second) != ptr_slider->getValue())
                    ptr_slider->setValue(std::get<TupleValues::currentValue>(*it->second));
            
            Button *ptr_button = dynamic_cast<Button *>(it->first);
            if (ptr_button != nullptr && it->second != nullptr)
                if ((std::get<TupleValues::currentValue>(*it->second) != 0) ^ ptr_button->getToggleState())
                    ptr_button->setToggleState(std::get<TupleValues::currentValue>(*it->second) != 0, dontSendNotification);
        }
    }
    void buttonClicked(Button *button)
    {
        if (!links[button])
            return;
        if (button->getToggleState())
            std::get<TupleValues::currentValue>(*links[button]) = 1.0;
        else
            std::get<TupleValues::currentValue>(*links[button]) = 0;
    };
    void sliderValueChanged(Slider *slider)
    {
        if (!links[slider])
            return;
        std::get<TupleValues::currentValue>(*links[slider]) = (float)slider->getValue()/10;
    };
	
protected:
    std::map<Component *, rmpEffect::valueTuple *> links;
    globalEffectRackLookAndFeel lookAndFeel;
};
