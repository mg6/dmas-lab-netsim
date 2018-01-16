#include <omnetpp.h>
#include <queue.h>
#include "global_stats_listener.h"
using namespace omnetpp;

class Sdn : public cSimpleModule {
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
    virtual void refreshDisplay() const override;

    void TryToSendMessage();
    virtual ~Sdn();
    Queue queue;
    simtime_t getInterfaceDelay();
    bool isSendingMessages=false;
    char selfString[2]="s";

    unsigned long numPacketsDropped;
    unsigned long numPacketsReceived;

    GlobalStatsListener* globalStats;
};

Define_Module(Sdn);

void Sdn::initialize(){
    numPacketsDropped = 0;
    numPacketsReceived = 0;

    cModule * mod = getModuleByPath("total_stats");
    if (mod) {
        globalStats = dynamic_cast<GlobalStatsListener*>(mod);
    } else {
        error("No total_stats module.");
    }
}

Sdn::~Sdn() {
    Packet* p;
    while ((p = queue.dequeue()) != NULL)
        delete p;
}

void Sdn::handleMessage(cMessage *msg){
    if(strncmp(msg->getName(), selfString, 10 )==0) //self message
    {
      delete(msg);
      TryToSendMessage();
      return;
    }

    ++numPacketsReceived;

    if(!queue.IsFull())
    {
        Packet *pack = check_and_cast<Packet*>(msg);
        queue.enqueue(pack);

        if(!isSendingMessages)
        {
            isSendingMessages=true;
            TryToSendMessage();
        }
    }
    else
    {
        delete msg;
        ++numPacketsDropped;
        ++globalStats->getNumTotalDropped();
    }
}

void Sdn::TryToSendMessage(){
    simtime_t txEnd = getInterfaceDelay();
                if(txEnd <= simTime()){
                    send(queue.dequeue(), "out");
                    if(!queue.IsEmpty()){
                        simtime_t txEnd = getInterfaceDelay();
                        cMessage *self = new cMessage("self");
                            self->setName(selfString);
                        scheduleAt(txEnd>simTime()?txEnd:simTime(), self);
                                        }
                    else {
                        isSendingMessages=false;
                    }
                }
                else
                {
                    TryToSendMessage();
                }
}

simtime_t Sdn::getInterfaceDelay(){
            simtime_t txFT;
            cChannel *txChn = gate("out")->getTransmissionChannel();
            txFT = txChn->getTransmissionFinishTime();
             return txFT;
}

void Sdn::refreshDisplay() const {
    char buf[40];
    sprintf(buf, "rcvd: %lu drpd: %lu", numPacketsReceived, numPacketsDropped);
    getDisplayString().setTagArg("t", 0, buf);
}
