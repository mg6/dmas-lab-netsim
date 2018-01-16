#include <omnetpp.h>
#include <queue.h>
using namespace omnetpp;

class Sdn : public cSimpleModule {
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
    void TryToSendMessage();
    virtual ~Sdn();
    Queue queue;
    simtime_t getInterfaceDelay();
    bool isSendingMessages=false;
    char selfString[2]="s";
};

Define_Module(Sdn);

void Sdn::initialize(){
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
    }
else if(!queue.IsFull())
                   {

                       Packet *pack = check_and_cast<Packet*>(msg);
                       queue.enqueue(pack);
                       if(!isSendingMessages)
                       {
                           isSendingMessages=true;
                           TryToSendMessage();
                       }
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




