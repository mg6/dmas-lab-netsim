#include <omnetpp.h>

using namespace omnetpp;

class Sink : public cSimpleModule {
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
};

Define_Module(Sink);

void Sink::initialize(){

}

void Sink::handleMessage(cMessage *msg){

}
