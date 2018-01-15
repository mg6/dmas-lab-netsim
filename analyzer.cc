#include <omnetpp.h>
#include "queue.h"
#include <string>
#include "notification_m.h"
#include "packet_m.h"

using namespace omnetpp;
#define usInS 1000000
class Analyzer : public cSimpleModule {
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
    virtual void refreshDisplay() const override;
    void StartPackedAnalyze(cMessage *msg);
    void SendAnalyzedMessageFurther(cMessage *msg);
    void DisplayPackagesData();
    float analyzeTime;
    unsigned long receivedMessages = 0;
    unsigned long droppedMessages=0;
    unsigned long analyzedMessages=0;
    unsigned long startedAnalyzeMessages=0;
    Queue queue;
    char analyze[2]="a";

};

Define_Module(Analyzer);

void Analyzer::initialize(){
    int usTime=par("analyzeTime");
analyzeTime=(float)usTime/usInS;
EV_INFO << "t: " << analyzeTime<< endl;
}


void Analyzer::handleMessage(cMessage *msg)
{
    ++receivedMessages;
   if(strncmp(msg->getName(), analyze, 10 )==0)    //if(msg->isSelfMessage())
    {
       SendAnalyzedMessageFurther(msg);
    }
    else
    { //receiving from sdn
        DisplayPackagesData();

               if(!queue.IsFull())
               {
                   Packet *pack = check_and_cast<Packet*>(msg);
                   queue.enqueue(pack);
                   StartPackedAnalyze(msg);
               }
               else
               {
                   droppedMessages++;
                   EV_INFO << "dropped packed" << endl;
               }
    }

}

void Analyzer::StartPackedAnalyze(cMessage *msg)
{
    EV_INFO << "Started packed analyze" << endl;
    startedAnalyzeMessages++;
    cMessage *delayEvent = new cMessage("delayEvent");
    delayEvent->setName(analyze);
    scheduleAt(simTime()+analyzeTime, delayEvent);
}
void Analyzer::SendAnalyzedMessageFurther(cMessage *msg)
{
        analyzedMessages++;
       cancelEvent(msg);
       send(queue.dequeue(), "out");
       EV_INFO << "ended analyze " << endl;
}
void Analyzer::DisplayPackagesData()
{
        EV_INFO << "received fresh data" << endl;
        EV_INFO << "dropped: " <<droppedMessages<< endl;
        EV_INFO << "analyzed: " <<analyzedMessages<< endl;
        EV_INFO << "AnalyzeMessages: " <<startedAnalyzeMessages<< endl;
}

void Analyzer::refreshDisplay() const {
    char buf[40];
    sprintf(buf, "rcvd: %lu drpd: %lu", receivedMessages, droppedMessages);
    getDisplayString().setTagArg("t", 0, buf);
}
