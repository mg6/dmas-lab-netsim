#include <omnetpp.h>

using namespace omnetpp;

class Acx_pwr : public cSimpleModule {
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
};

Define_Module(Acx_pwr);

void Acx_pwr::initialize(){

}

void Acx_pwr::handleMessage(cMessage *msg){

}
