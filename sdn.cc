#include <omnetpp.h>

using namespace omnetpp;

class Sdn : public cSimpleModule {
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
};

Define_Module(Sdn);

void Sdn::initialize(){

}

void Sdn::handleMessage(cMessage *msg){

}
