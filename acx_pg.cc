#include <omnetpp.h>
#include "global_stats_listener.h"

using namespace omnetpp;

class Acx_pg : public cSimpleModule {
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
    virtual void refreshDisplay() const override;

    unsigned long numPacketsDropped;
    unsigned long numPacketsReceived;

    GlobalStatsListener* globalStats;
};

Define_Module(Acx_pg);

void Acx_pg::initialize(){
    numPacketsDropped = 0;
    numPacketsReceived = 0;

    cModule * mod = getModuleByPath("total_stats");
    if (mod) {
        globalStats = dynamic_cast<GlobalStatsListener*>(mod);
    } else {
        error("No total_stats module.");
    }
}

void Acx_pg::handleMessage(cMessage *msg){
    ++numPacketsReceived;

    EV<<"Routing error - message on ACX_PG node";

    delete(msg);
    ++numPacketsDropped;
    ++globalStats->getNumTotalDropped();
}

void Acx_pg::refreshDisplay() const {
    char buf[40];
    sprintf(buf, "rcvd: %lu drpd: %lu", numPacketsReceived, numPacketsDropped);
    getDisplayString().setTagArg("t", 0, buf);
}
