
#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include <unordered_set>

class StartStopBroadcaster
{
public:
    StartStopBroadcaster() = default;
    ~StartStopBroadcaster() = default;
    class Listener
    {
    public:
        virtual void bcStarted(StartStopBroadcaster &) = 0;
        virtual bool bcFinishing(StartStopBroadcaster &) = 0;
        virtual void delayedFinish() = 0;
    protected:
        std::list<StartStopBroadcaster *> locked_bcs;
    };
    void addListener(Listener *listener)
    {
        listeners.insert(listener);
    };
    void clearListeners()
    {
        listeners.clear();
    };
    void sendToListenersAboutStart()
    {
        for (Listener *listener : listeners)
            listener->bcStarted(*this);
    };
    virtual void reactOnDelayedStop() = 0;
    bool askListenersForRelease()
    {
        for (Listener *listener : listeners)
            if (!listener->bcFinishing(*this))
                return false;
        return true;
    };

protected:
    std::unordered_set<Listener *> listeners;
};