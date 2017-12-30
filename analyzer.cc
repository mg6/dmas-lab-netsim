#include <omnetpp.h>

using namespace omnetpp;

class Analyzer : public cSimpleModule {
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
};

Define_Module(Analyzer);

void Analyzer::initialize(){

}

void Analyzer::handleMessage(cMessage *msg){

}
