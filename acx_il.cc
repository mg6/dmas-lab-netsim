#include <omnetpp.h>

using namespace omnetpp;

class Acx_il : public cSimpleModule {
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
};

Define_Module(Acx_il);

void Acx_il::initialize(){

}

void Acx_il::handleMessage(cMessage *msg){

}
