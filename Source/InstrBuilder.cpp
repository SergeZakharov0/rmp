
#include "InstrBuilder.h"
#include <memory>

rmpSynth *InstrBuilder::parseInstr(int numberOfVoicesToCreate, CriticalSection &_lock)
{
    rmpSynth *synth = new rmpSynth(_lock);
    std::shared_ptr<SummedSound> sound = std::make_shared<SummedSound>();
    synth->sound = sound;
    std::list<std::shared_ptr<SummedVoice>> &voices = synth->voices;
    for (int i = 0; i < numberOfVoicesToCreate; ++i)
        voices.push_back(std::make_shared<SummedVoice>(*sound));

    forEachXmlChildElement(*instrConfig, instr_item) {
        if (instr_item->hasTagName("layer"))
        {
            // Allocation
            std::shared_ptr<LayerSound> lsound = std::make_shared<LayerSound>();
            std::list< std::shared_ptr<LayerVoice>> lvoices;
            for (int i = 0; i < voices.size(); ++i)
                lvoices.push_back(std::make_shared<LayerVoice>(*lsound));
            
            // Parsing
            parseLayer(instr_item, lsound, lvoices);

            // Attaching
            sound->layerSounds.push_back(lsound);
            auto lvoice = lvoices.begin();
            for (auto ivoice = voices.begin(); ivoice != voices.end(); ++ivoice)
            {
                ivoice->get()->layerVoices.push_back(*lvoice);
                ++lvoice;
            }
        }
        if (instr_item->hasTagName("effects"))
        {
            // Allocating
            std::shared_ptr<rmpEffectRack> soundRack = std::make_shared<rmpEffectRack>();
            std::list<std::shared_ptr<rmpEffectRack>> voiceRacks;
            for (int i = 0; i < (voices.size() * sound->layerSounds.size()); ++i)
                voiceRacks.push_back(std::make_shared<rmpEffectRack>());
            
            // Find all subracks
            std::vector<rmpEffectRack *> subRacks;
            for (auto it = sound->layerSounds.begin(); it != sound->layerSounds.end(); ++it)
                subRacks.push_back((*it)->rack.get());

            // Parsing
            parseRack(instr_item, soundRack, voiceRacks, subRacks);
            
            // Attaching
            sound->rack = soundRack;
            auto voicerack = voiceRacks.begin();
            for (auto ivoice = voices.begin(); ivoice != voices.end(); ++ivoice)
            {
                for (auto lvoice = (*ivoice)->layerVoices.begin(); lvoice != (*ivoice)->layerVoices.end(); ++lvoice, ++voicerack)
                    lvoice->get()->rack = *voicerack;
                ivoice->get()->rack = std::make_shared<rmpEffectRack>();
            }
        }
    }
    for (auto ivoice = voices.begin(); ivoice != voices.end(); ++ivoice)
    {
        for (auto lvoice = (*ivoice)->layerVoices.begin(); lvoice != (*ivoice)->layerVoices.end(); ++lvoice)
            lvoice->get()->repairRackLinks();
        ivoice->get()->repairRackLinks();
    }
    return synth;
}

void InstrBuilder::parseLayer(XmlElement *layerConfig, std::shared_ptr<LayerSound> lsound, std::list<std::shared_ptr<LayerVoice>> lvoices)
{
    forEachXmlChildElement(*layerConfig, layer_item) {
        if (layer_item->hasTagName("name"))
            lsound->name = layer_item->getAllSubText();
        if (layer_item->hasTagName("box")) {
            soundBox tempBox;
            forEachXmlChildElement(*layer_item, params_item) {
                if (params_item->hasTagName("mainnote"))
                    tempBox.mainNote = (uint8)params_item->getAllSubText().getIntValue();
                if (params_item->hasTagName("lowestnote"))
                    tempBox.lowestNote = (uint8)params_item->getAllSubText().getIntValue();
                if (params_item->hasTagName("highestnote"))
                    tempBox.highestNote = (uint8)params_item->getAllSubText().getIntValue();
                if (params_item->hasTagName("mainvel"))
                    tempBox.mainVel = (uint8)params_item->getAllSubText().getIntValue();
                if (params_item->hasTagName("lowestvel"))
                    tempBox.lowestVel = (uint8)params_item->getAllSubText().getIntValue();
                if (params_item->hasTagName("highestvel"))
                    tempBox.highestVel = (uint8)params_item->getAllSubText().getIntValue();
                if (params_item->hasTagName("transpose"))
                    tempBox.transposeMethod = params_item->getAllSubText();
                if (params_item->hasTagName("soundfile")) {
                    String soundfile = String(params_item->getAllSubText());
                    MemoryInputStream *stream = (MemoryInputStream *)source->createInputStreamFor(soundfile);
                    tempBox.soundfile_size = stream->getDataSize();
                    tempBox.soundfile_data = malloc(tempBox.soundfile_size);
                    memcpy(tempBox.soundfile_data, stream->getData(), tempBox.soundfile_size);
                    delete stream;
                }
            }
            lsound->appendBox(tempBox, hostSampleRate);
        }
        if (layer_item->hasTagName("effects"))
        {
            // Allocating
            std::shared_ptr<rmpEffectRack> soundRack = std::make_shared<rmpEffectRack>();
            std::list<std::shared_ptr<rmpEffectRack>> voiceRacks;
            for (int i = 0; i < lvoices.size(); ++i)
                voiceRacks.push_back(std::make_shared<rmpEffectRack>());

            // Parsing
            parseRack(layer_item, soundRack, voiceRacks);

            // Attaching
            lsound->rack = soundRack;
        }
    }
}

void InstrBuilder::parseRack(XmlElement *rackConfig, std::shared_ptr<rmpEffectRack> soundRack, std::list<std::shared_ptr<rmpEffectRack>> voiceRacks, std::vector<rmpEffectRack *> subRacks)
{
    forEachXmlChildElement(*rackConfig, effect_item)
    {
        if (effect_item->hasTagName("volume"))
        {
            String _name = "volume" + String(soundRack->getRackSize() + 1);
            std::shared_ptr<rmpVolume> eff = std::make_shared<rmpVolume>(_name, hostSampleRate);
            eff->setSingleParam("value", effect_item->getChildByName("value")->getAllSubText().getFloatValue());
            soundRack->addEffect(_name, eff);
        }
        if (effect_item->hasTagName("pan"))
        {
            String _name = "pan" + String(soundRack->getRackSize() + 1);
            std::shared_ptr<rmpPan> eff = std::make_shared<rmpPan>(_name, hostSampleRate);
            eff->setSingleParam("value", effect_item->getChildByName("value")->getAllSubText().getFloatValue());
            soundRack->addEffect(_name, eff);
        }
        if (effect_item->hasTagName("reverb"))
        {
            String _name = "reverb" + String(soundRack->getRackSize() + 1);
            std::shared_ptr<rmpReverb> eff = std::make_shared<rmpReverb>(_name, hostSampleRate);
            eff->setSingleParam("dryWet", effect_item->getChildByName("dryWet")->getAllSubText().getFloatValue());
            eff->setSingleParam("roomSize", effect_item->getChildByName("roomSize")->getAllSubText().getFloatValue());
            eff->setSingleParam("width", effect_item->getChildByName("width")->getAllSubText().getFloatValue());
            soundRack->addEffect(_name, eff);
        }
        if (effect_item->hasTagName("delay"))
        {
            String _name = "delay" + String(soundRack->getRackSize() + 1);
            std::shared_ptr<rmpDelay> eff = std::make_shared<rmpDelay>(_name, hostSampleRate);
            eff->setSingleParam("dryWet", effect_item->getChildByName("dryWet")->getAllSubText().getFloatValue());
            eff->setSingleParam("time", effect_item->getChildByName("time")->getAllSubText().getFloatValue());
            eff->setSingleParam("feedback", effect_item->getChildByName("feedback")->getAllSubText().getFloatValue());
            soundRack->addEffect(_name, eff);
        }
        if (effect_item->hasTagName("adsr"))
        {
            String _name = "adsr" + String(soundRack->getRackSize() + 1);

            std::list<std::shared_ptr<rmpADSR>> voiceeff;
            for (int i = 0; i < voiceRacks.size(); ++i)
                voiceeff.push_back(std::make_shared<rmpADSR>(_name, hostSampleRate));
            std::shared_ptr<rmpMirrorController> contr = std::make_shared<rmpMirrorController>(_name, **voiceeff.begin(), hostSampleRate);
            for (auto it = voiceeff.begin(); it != voiceeff.end(); ++it)
                contr->linkRack(*it);

            contr->setSingleParam("attack", effect_item->getChildByName("attack")->getAllSubText().getFloatValue());
            contr->setSingleParam("decay", effect_item->getChildByName("decay")->getAllSubText().getFloatValue());
            contr->setSingleParam("sustain", effect_item->getChildByName("sustain")->getAllSubText().getFloatValue());
            contr->setSingleParam("release", effect_item->getChildByName("release")->getAllSubText().getFloatValue());
            contr->setSingleParam("broken", 0);

            soundRack->addEffect(_name, contr);
            auto effit = voiceeff.begin();
            for (auto rackit = voiceRacks.begin(); rackit != voiceRacks.end(); ++rackit, ++effit)
            {
                rackit->get()->addEffect(_name, *effit);
            }
        }
        if (effect_item->hasTagName("func"))
        {
            String _name = "func" + String(soundRack->getRackSize() + 1);

            std::shared_ptr<rmpFunctionalController> contr = std::make_shared<rmpFunctionalController>(_name, hostSampleRate);
            forEachXmlChildElementWithTagName(*effect_item, link_item, "link")
            {
                rmpEffectRack * subRack = 0;
                rmpEffect *effect = 0;
                String name = "";
                rmpFunctionalController::Law *law = 0;
                forEachXmlChildElement(*link_item, param_item)
                {
                    if (param_item->hasTagName("layer"))
                    {
                        int value = param_item->getAllSubText().getIntValue();
                        if (value <= 0)
                            subRack = soundRack.get();
                        else
                            if (value <= subRacks.size())
                                subRack = subRacks[value - 1];
                            else
                                throw;
                    }
                    if (param_item->hasTagName("namepart"))
                    {
                        effect = subRack->findEffect(param_item->getAllSubText());
                    }
                    if (param_item->hasTagName("paramname"))
                    {
                        name = param_item->getAllSubText();
                    }
                    if (param_item->hasTagName("law"))
                    {
                        String lawName = param_item->getAllSubText();;
                        if (lawName == "default")
                            law = new rmpFunctionalController::Law();
                        if (lawName == "inverse")
                            law = new rmpFunctionalController::inverseLaw();
                    }
                }
                contr->link(rmpFunctionalController::Parameter(name, effect, law));
            }
            contr->setSingleParam("value", effect_item->getChildByName("value")->getAllSubText().getFloatValue());
            soundRack->addEffect(_name, contr);
            
        }
    }
}
