/*
  ==============================================================================

    LockingVoice.h
    Created: 20 Mar 2019 1:49:47pm
    Author:  serge

  ==============================================================================
*/

#pragma once


#include "../JuceLibraryCode/JuceHeader.h"
#include <list>
#include <unordered_set>

class LockingVoice : public SynthesiserVoice
{
public:
    LockingVoice() = default;
    ~LockingVoice() = default;
    class Listener
    {
    public:
        virtual void voiceStarted(LockingVoice *) = 0;
        virtual bool voiceAskingForRelease(LockingVoice *) = 0;
        virtual void delayedRelease(LockingVoice *) = 0;
    protected:
        std::list<SynthesiserVoice *> locked_entries;
    };
    void addLockingListener(Listener *listener)
    {
        listeners.insert(listener);
    };
    void clearLockingListeners()
    {
        listeners.clear();
    };
    void sendToListenersAboutStart()
    {
        for (Listener *listener : listeners)
            listener->voiceStarted(this);
    };
    bool askListenersForRelease()
    {
        for (Listener *listener : listeners)
            if (!listener->voiceAskingForRelease(this))
                return false;
        return true;
    };

protected:
    std::unordered_set<Listener *> listeners;
};