#include <omnetpp.h>

using namespace omnetpp;

class Acx_pcss : public cSimpleModule {
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
};

Define_Module(Acx_pcss);

void Acx_pcss::initialize(){

}

void Acx_pcss::handleMessage(cMessage *msg){

}
