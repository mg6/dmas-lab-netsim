#include <omnetpp.h>
#include "global_stats_listener.h"

using namespace omnetpp;

class Sink : public cSimpleModule {
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
    virtual void refreshDisplay() const override;

private:
    GlobalStatsListener* globalStats;
    unsigned long numReceived;
};

Define_Module(Sink);

void Sink::initialize(){
    numReceived = 0;

    cModule * mod = getModuleByPath("total_stats");
    if (mod) {
        globalStats = dynamic_cast<GlobalStatsListener*>(mod);
    } else {
        error("No global_stats module.");
    }
}

void Sink::handleMessage(cMessage *msg){
    ++numReceived;
    ++globalStats->getNumTotalDelivered();

    delete msg;
}

void Sink::refreshDisplay() const {
    char buf[40];
    sprintf(buf, "rcvd: %lu", numReceived);
    getDisplayString().setTagArg("t", 0, buf);
}
