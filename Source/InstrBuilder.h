
#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "rmpSynth.h"
#include "EffectRack.h"
#include "SQLInputSource.h"

class InstrBuilder
{
public:
    InstrBuilder(XmlElement *_instrConfig, SQLInputSource *_source, float _hostSampleRate)
    {
        instrConfig = _instrConfig;
        source = _source;
        hostSampleRate = _hostSampleRate;
    }
    ~InstrBuilder() = default;

    rmpSynth *parseInstr(int numberOfVoicesToCreate);
protected:
    void parseLayer(XmlElement *layerConfig, std::shared_ptr<LayerSound> lsound, std::list<std::shared_ptr<LayerVoice>> lvoices);
    void parseRack(XmlElement *rackConfig, std::shared_ptr<rmpEffectRack> soundRack, std::list<std::shared_ptr<rmpEffectRack>> voiceRacks);
private:
    XmlElement *instrConfig;
    SQLInputSource *source;
    float hostSampleRate;
};