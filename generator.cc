#include <omnetpp.h>

using namespace omnetpp;

class Generator : public cSimpleModule {
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
};

Define_Module(Generator);

void Generator::initialize(){

}

void Generator::handleMessage(cMessage *msg){

}
