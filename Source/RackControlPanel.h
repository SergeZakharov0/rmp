
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
        for (auto it = links.begin(); it != links.end(); ++it)
            delete(it->first);
    }

    void addRotarySlider(rmpEffect *eff, String param, Rectangle<int> bounds, String name, Image)
    {
        Slider *slider = new Slider(Slider::SliderStyle::RotaryHorizontalVerticalDrag, Slider::TextEntryBoxPosition::NoTextBox);
        slider->setName(name);
        slider->setBounds(bounds);
        addAndMakeVisible(*slider);
        links.emplace(slider, std::pair<rmpEffect *, String>(eff, param));
        slider->addListener(this);
        if (eff != nullptr && param != "")
            slider->setMinAndMaxValues(std::get<TupleValues::minimalValue>(eff->getParams()[param]), std::get<TupleValues::maximalValue>(eff->getParams()[param]));
    }
    void addHorizontalSlider(rmpEffect *eff, String param, Rectangle<int> bounds, String name, Image)
    {
        Slider *slider = new Slider(Slider::SliderStyle::LinearHorizontal, Slider::TextEntryBoxPosition::NoTextBox);
        slider->setName(name);
        slider->setBounds(bounds);
        addAndMakeVisible(*slider);
        links.emplace(slider, std::pair<rmpEffect *, String>(eff, param));
        slider->addListener(this);
        if (eff != nullptr && param != "")
            slider->setMinAndMaxValues(std::get<TupleValues::minimalValue>(eff->getParams()[param]), std::get<TupleValues::maximalValue>(eff->getParams()[param]));
    }
    void addButton(rmpEffect *eff, String param, Rectangle<int> bounds, String name, Image)
    {
        Button *button = new rmpButton();
        button->setName(name);
        button->setBounds(bounds);
        addAndMakeVisible(*button);
        links.emplace(button, std::pair<rmpEffect *, String>(eff, param));
        button->addListener(this);
        if (eff != nullptr && param != "")
            button->setToggleState(std::get<TupleValues::currentValue>(eff->getParams()[param]) != 0, dontSendNotification);
    }
    void setLink(rmpEffect *eff, String param, String _componentName)
    {
        for (auto it = links.begin(); it != links.end(); ++it)
            if (it->first->getName() == _componentName)
            {
                it->second = std::pair<rmpEffect *, String>(eff, param);

                if (eff != nullptr && param != "")
                {
                    Slider *ptr_slider = dynamic_cast<Slider *>(it->first);
                    if (ptr_slider)
                    {
                        if (ptr_slider->getSliderStyle() == Slider::SliderStyle::LinearHorizontal)
                            ptr_slider->setMinAndMaxValues(std::get<TupleValues::minimalValue>(eff->getParams()[param]), std::get<TupleValues::maximalValue>(eff->getParams()[param]));
                        ptr_slider->setValue(std::get<TupleValues::currentValue>(eff->getParams()[param])*10);
                    }

                    Button *ptr_button = dynamic_cast<Button *>(it->first);
                    if (ptr_button)
                    {
                        ptr_button->setToggleState(std::get<TupleValues::currentValue>(eff->getParams()[param]) != 0, dontSendNotification);
                    }
                }
            }
    }
    void EffectParamsChanged(rmpEffect &) 
    {
        for (auto it = links.begin(); it != links.end(); ++it)
        {
            Slider *ptr_slider = dynamic_cast<Slider *>(it->first);
            if (ptr_slider != nullptr && it->second.first != nullptr)
                if (it->second.first->getParamValue(it->second.second) != ptr_slider->getValue())
                    ptr_slider->setValue(it->second.first->getParamValue(it->second.second));
            
            Button *ptr_button = dynamic_cast<Button *>(it->first);
            if (ptr_button != nullptr && it->second.first != nullptr)
                if ((it->second.first->getParamValue(it->second.second) != 0) ^ ptr_button->getToggleState())
                    ptr_button->setToggleState(it->second.first->getParamValue(it->second.second) != 0, dontSendNotification);
        }
    }
    void buttonClicked(Button *button)
    {
        if (!links[button].first)
            return;
        if (button->getToggleState())
            links[button].first->setSingleParam(links[button].second, 1.0);
        else
            links[button].first->setSingleParam(links[button].second, 0.0);
    };
    void sliderValueChanged(Slider *slider)
    {
        if (!links[slider].first)
            return;
        links[slider].first->setSingleParam(links[slider].second, (float)slider->getValue() / 10);
    };
	
protected:
    std::map<Component *, std::pair<rmpEffect *, String>> links;
    globalEffectRackLookAndFeel lookAndFeel;
};
