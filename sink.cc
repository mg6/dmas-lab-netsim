#include <omnetpp.h>

using namespace omnetpp;

class Sink : public cSimpleModule {
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
    virtual void refreshDisplay() const override;

private:
    unsigned long numReceived;
};

Define_Module(Sink);

void Sink::initialize(){
    numReceived = 0;
}

void Sink::handleMessage(cMessage *msg){
    ++numReceived;
}

void Sink::refreshDisplay() const {
    char buf[40];
    sprintf(buf, "rcvd: %lu", numReceived);
    getDisplayString().setTagArg("t", 0, buf);
}
