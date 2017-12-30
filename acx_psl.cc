#include <omnetpp.h>

using namespace omnetpp;

class Acx_psl : public cSimpleModule {
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
};

Define_Module(Acx_psl);

void Acx_psl::initialize(){

}

void Acx_psl::handleMessage(cMessage *msg){

}
