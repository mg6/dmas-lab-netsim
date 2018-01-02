#include <omnetpp.h>
#include "packet_m.h"

using namespace omnetpp;

class Generator : public cSimpleModule {
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;

    int packetLength;
    int sendRate;
    int destination;
};

Define_Module(Generator);

void Generator::initialize(){
    packetLength = par("packetLength");
    sendRate = par("sendRate");
    destination = 0;
    cMessage *selfMessage = new cMessage();
    scheduleAt(simTime(), selfMessage);
}

void Generator::handleMessage(cMessage *msg){
    Packet *pkt = new Packet();
    pkt->setDestinationAddress(destination);
    pkt->setByteLength((int64_t)packetLength);
    cChannel *outChannel = gate("out")->getTransmissionChannel();
    simtime_t finishTime = outChannel->getTransmissionFinishTime();
    if(finishTime <= simTime())
    {
        send(pkt, "out");
        EV << "Packet sent to " << pkt->getDestinationAddress() << " at time " << simTime() << ".\n";
        destination = (++destination)%4;
        scheduleAt(simTime(), msg);
    }
    else
    {
        delete pkt;
        scheduleAt(finishTime, msg);
    }
}
