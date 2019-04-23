
#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

#include "EffectRack.h"
#include <map>


class globalEffectRackLookAndFeel : public LookAndFeel_V4 {
public:
    void drawLinearSlider(Graphics &g, int x, int y, int width, int height, float, float, float, const Slider::SliderStyle, Slider &slider) {
/*        Image bg = ImageCache::getFromMemory(BinaryData::librarysliderback_png, BinaryData::librarysliderback_pngSize);
        g.drawImage(bg, x + width / 4, y, width / 2, height + y, 0, 0, bg.getWidth(), bg.getHeight());
        Image head = ImageCache::getFromMemory(BinaryData::librarysliderhead_png, BinaryData::librarysliderhead_pngSize);
        float percentPos = (float)slider.getValue();
        g.drawImage(head, x, (int)(y + (1 - percentPos) * (height - head.getHeight() / 2)), width, width, 0, 0, head.getWidth(), head.getHeight()); */
    };
    void drawToggleButton(Graphics &g, ToggleButton &b, bool, bool)
    {
        Image buttonactiveimage;
        if (b.getToggleState())
            buttonactiveimage = ImageCache::getFromMemory(BinaryData::buttonactive_png, BinaryData::buttonactive_pngSize);
        else
            buttonactiveimage = ImageCache::getFromMemory(BinaryData::buttoninactive_png, BinaryData::buttoninactive_pngSize);
        Rectangle<int> now = b.getBounds();
        g.drawImage(buttonactiveimage, Rectangle<float>(0, 0, now.getWidth(), now.getHeight()), RectanglePlacement::fillDestination);
    };
    void drawRotarySlider(Graphics& g, int, int, int width, int height, float sliderPos,
        const float rotaryStartAngle, const float rotaryEndAngle, Slider&) override
    { 
        Image fixed = ImageCache::getFromMemory(BinaryData::rotarybackground_png, BinaryData::rotarybackground_pngSize);
        g.drawImage(fixed, Rectangle<float>(0, 0, (float)width, (float)height), RectanglePlacement::fillDestination);

        Image pointer = ImageCache::getFromMemory(BinaryData::rotaryhead_png, BinaryData::rotaryhead_pngSize);
        AffineTransform transform = AffineTransform().rotated((rotaryEndAngle - rotaryStartAngle) * sliderPos + rotaryStartAngle, (float)pointer.getWidth() / 2, (float)pointer.getHeight() / 2).scaled((float)width / (float)pointer.getWidth());
        g.drawImageTransformed(pointer, transform);
    }

};

class rmpButton : public ToggleButton
{
public:
    rmpButton(String _name = "") : ToggleButton(_name) {};
    void clicked() override 
    {
    }
};

class EffectControlPanel : public Component, public rmpEffect::Listener, public ToggleButton::Listener, public Slider::Listener
{
public:
    EffectControlPanel()
    {
        setLookAndFeel(&lookAndFeel);
        setPaintingIsUnclipped(true);
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
        {
            slider->setMinAndMaxValues(std::get<TupleValues::minimalValue>(eff->getParams()[param]), std::get<TupleValues::maximalValue>(eff->getParams()[param]));
            eff->addListener(this);
        }
    }
    void addButton(rmpEffect *eff, String param, Rectangle<int> bounds, String name, Image)
    {
        ToggleButton *button = new rmpButton("");
        button->setName(name);
        button->setBounds(bounds);
        button->setPaintingIsUnclipped(true);
        addAndMakeVisible(*button);
        links.emplace(button, std::pair<rmpEffect *, String>(eff, param));
        button->addListener(this);
        if (eff != nullptr && param != "")
        {
            button->setToggleState(std::get<TupleValues::currentValue>(eff->getParams()[param]) != 0, dontSendNotification);
            eff->addListener(this);
        }
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
                        eff->addListener(this);
                    }

                    rmpButton *ptr_button = dynamic_cast<rmpButton *>(it->first);
                    if (ptr_button)
                    {
                        ptr_button->setToggleState(std::get<TupleValues::currentValue>(eff->getParams()[param]) != 0, dontSendNotification);
                        eff->addListener(this);
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
                {
                    float now = it->second.first->getParamValue(it->second.second);
                    ptr_slider->setValue(now*10);
                }
            
            rmpButton *ptr_button = dynamic_cast<rmpButton *>(it->first);
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
