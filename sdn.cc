#include <omnetpp.h>
#include <queue.h>
using namespace omnetpp;

class Sdn : public cSimpleModule {

    cMessage *event;  // pointer to the event object which we'll use for timing
     cMessage *tictocMsg;  // variable to remember the message until we send it back

    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
};

Define_Module(Sdn);

void Sdn::initialize(){
    cMessage *selfMessage = new cMessage();
}

void Sdn::handleMessage(cMessage *msg){
    send(msg, "out");
   // msg->getSenderModuleId();
//if(strncmp(msg->getName(), "ImEmpty", 10))
    /*
        if(  msg->getSenderModuleId()==12)
    {
       canSend=true;
    }
    else
    {
        Packet *pack = check_and_cast<Packet*>(msg);
    queue.enqueue(pack);
    }

    if(canSend)
    {
        canSend=false;
        send(queue.dequeue(), "out");
    }

*/
}
