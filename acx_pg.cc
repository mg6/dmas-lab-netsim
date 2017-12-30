#include <omnetpp.h>

using namespace omnetpp;

class Acx_pg : public cSimpleModule {
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
};

Define_Module(Acx_pg);

void Acx_pg::initialize(){

}

void Acx_pg::handleMessage(cMessage *msg){

}
