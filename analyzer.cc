#include <omnetpp.h>
#include "queue.h"
#include <string>
#include "notification_m.h"
#include "packet_m.h"

using namespace omnetpp;


class Analyzer : public cSimpleModule {
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
    simtime_t getInterfaceDelay(int destination);
    simtime_t analyzeTime=0; //make it in inifile
    simtime_t delay=0.00001;
    bool isAnalyzing=false;
   // cMessage *delayEvent;  // holds pointer to the delay self-message
    cMessage *analyzedPackage;  // holds pointer to the analyzed image
   // cMessage *imEmpty;  // holds pointer to the analyzed image
    int droppedMessages=0;
        Queue queue;
        bool canSend=true;
        char name[2]="d";// = "de";
        char overload[2]="o";
};

Define_Module(Analyzer);

void Analyzer::initialize(){
   // name[0]="d";
   // name[1]="e";
    //send(imEmpty, "in");
  //  delayEvent = new cMessage("delayEvent");
   // imEmpty=new cMessage("ImEmpty");
}


simtime_t Analyzer::getInterfaceDelay(int destination){
    simtime_t txFT;
            cChannel *txChn = gate("out")->getTransmissionChannel();
            txFT = txChn->getTransmissionFinishTime();
        return txFT;
}

void Analyzer::handleMessage(cMessage *msg)
{
    simtime_t txEnd;
    EV_INFO << msg->getName() << endl;
    if(strncmp(msg->getName(), overload, 10 )==0)
    {
        cancelEvent(msg);

        txEnd = getInterfaceDelay(queue.getFirstDestination());
                                   if(txEnd <= simTime()){
                                       send(queue.dequeue(), "out");
                                   }
                                   else
                                   {
                                       //Should NOT happen
                                                      EV<<"Unexpected load in outgoing channel - delaying";
                                                      cMessage *event = new cMessage("overload");  // holds pointer to the delay self-message
                                                      event->setName(overload);
                                                      scheduleAt(txEnd, event);
                                   }
                                   delete(msg);
    }
    else if(strncmp(msg->getName(), name, 10 )==0)    //if(msg->isSelfMessage())
    {
        cancelEvent(msg);
        EV_INFO << "ended analyze 0" << endl;
        //analyzedPackage=queue.dequeue();
         txEnd = getInterfaceDelay(queue.getFirstDestination());
                           if(txEnd <= simTime()){
                               EV_INFO << "ended analyze 1" << endl;
                               send(queue.dequeue(), "out");

                           }
                           else
                           {
                               //Should NOT happen
                                              EV<<"Unexpected load in outgoing channel - delaying";
                                              cMessage *event2 = new cMessage("overload");  // holds pointer to the delay self-message
                                                event2->setName(overload);
                                               scheduleAt(txEnd, event2);
                           }

       // isAnalyzing=false;
        analyzeTime+=delay;
        if(!queue.IsEmpty())
        {
            EV_INFO << "starting" << endl;
           // isAnalyzing=true;

                cMessage *delayEvent = new cMessage("delayEvent");  // holds pointer to the delay self-message
                //char name[] = "de";
                delayEvent->setName(name);
                scheduleAt(simTime()+delay, delayEvent);
        }
    }
    else
    { //receiving from sdn

        EV_INFO << "received" << endl;
             Packet *pack = check_and_cast<Packet*>(msg);
           queue.enqueue(pack);
           if(!isAnalyzing)
           {
               if(!queue.IsFull())
               {
                   EV_INFO << "starting" << endl;
                  // isAnalyzing=true;
               //analyzedPackage=check_and_cast<Packet*>(queue.dequeue());
               cMessage *delayEvent2 = new cMessage("delayEvent");
              // char name[] = "de";
               delayEvent2->setName(name);
                               scheduleAt(simTime()+delay, delayEvent2);
               }
               else
               {
                   EV_INFO << "dropped" << endl;
               }
           }
    }

}

