#include <omnetpp.h>

using namespace omnetpp;

class Acx_pw : public cSimpleModule {
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
};

Define_Module(Acx_pw);

void Acx_pw::initialize(){

}

void Acx_pw::handleMessage(cMessage *msg){

}
