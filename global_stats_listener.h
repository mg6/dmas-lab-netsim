#ifndef __GLOBAL_STATS_LISTENER_H__
#define __GLOBAL_STATS_LISTENER_H__

#include <omnetpp.h>
#include "packet_m.h"

using namespace omnetpp;

class GlobalStatsListener : public cSimpleModule {
public:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
    virtual void refreshDisplay() const override;
    virtual void finish() override;

    // unsigned long& getNumTotalGenerated() { return numTotalGenerated; }
    unsigned long& getNumTotalDropped() { return numTotalDropped; }
    // unsigned long& getNumTotalDroppedInAnalyzer() { return numTotalDroppedInAnalyzer; }
    unsigned long& getNumTotalDelivered() { return numTotalDelivered; }

private:
    // unsigned long numTotalGenerated; // displayed by generator
    unsigned long numTotalDropped;
    // unsigned long numTotalDroppedInAnalyzer; // displayed by analyzer
    unsigned long numTotalDelivered;
};

#endif // __GLOBAL_STATS_LISTENER_H__
