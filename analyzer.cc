#include <omnetpp.h>
#include "queue.h"
#include <string>
#include "notification_m.h"
#include "packet_m.h"

using namespace omnetpp;
#define usInS 1000000
class Analyzer : public cSimpleModule {
    virtual void initialize() override;
    virtual ~Analyzer();
    virtual void handleMessage(cMessage *msg) override;
    virtual void refreshDisplay() const override;
    void StartPackedAnalyze();
    void EndAnalyze();
    void DisplayPackagesData();
    bool isAnalyzing = false;
    float analyzeTime;
    unsigned long receivedMessages = 0;
    unsigned long droppedMessages = 0;
    unsigned long analyzedMessages = 0;
    unsigned long startedAnalyzeMessages = 0;
    Queue queue;
    char analyze[2] = "a";

    Queue queueOut;
    void TryToSendMessage();
    simtime_t getInterfaceDelay();
    bool isSendingMessages = false;
    char selfString[2] = "s";

};

Define_Module(Analyzer);

void Analyzer::initialize() {
    int usTime = par("analyzeTime");
    analyzeTime = (float)usTime / usInS;
    EV_INFO << "t: " << analyzeTime << endl;
}

Analyzer::~Analyzer() {
    Packet* p;
    while ((p = queue.dequeue()) != NULL)
        delete p;
}


void Analyzer::handleMessage(cMessage *msg)
{
    ++receivedMessages;
    EV_INFO << "0" << endl;
    if (strncmp(msg->getName(), analyze, 10) == 0)    //if(msg->isSelfMessage())
    {
        EV_INFO << "1" << endl;
        delete msg;
        EndAnalyze();
    }
    else if (strncmp(msg->getName(), selfString, 10) == 0)
    {
        EV_INFO << "2 " << endl;
        delete(msg);
        TryToSendMessage();
    }
    else
    { //receiving from sdn
        DisplayPackagesData();
        EV_INFO << "1" << endl;
        if (!queue.IsFull())
        {
            Packet *pack = check_and_cast<Packet*>(msg);
            queue.enqueue(pack);
            if (!isAnalyzing)
            {
                StartPackedAnalyze();
            }
        }
        else
        {
            droppedMessages++;
            EV_INFO << "dropped packed" << endl;
            delete msg;
        }
    }
    EV_INFO << "099" << endl;
}

void Analyzer::StartPackedAnalyze()
{
    EV_INFO << "4.6" << endl;
    isAnalyzing = true;
    EV_INFO << "Started packed analyze" << endl;
    startedAnalyzeMessages++;
    cMessage *delayEvent = new cMessage("delayEvent");
    delayEvent->setName(analyze);
    EV_INFO << "4.7" << endl;
    scheduleAt(simTime() + analyzeTime, delayEvent);
    EV_INFO << "4.8" << endl;
}
void Analyzer::EndAnalyze()
{
    analyzedMessages++;
    //send(queue.dequeue(), "out");
    EV_INFO << "3" << endl;



    if (!isSendingMessages && !queueOut.IsFull())
    {
        EV_INFO << "4" << endl;
        isSendingMessages = true;
        queueOut.enqueue(queue.dequeue());
        TryToSendMessage();
        EV_INFO << "10" << endl;
    }

    EV_INFO << "ended analyze " << endl;
    if (!queue.IsEmpty())
    {
        EV_INFO << "4.5" << endl;
        StartPackedAnalyze();
    }
    else
        isAnalyzing = false;
    //move it to the moment package is pushed forward as it is guarantee we can start another analyze


}

void Analyzer::TryToSendMessage() {
    simtime_t txEnd = getInterfaceDelay();
    if (txEnd <= simTime()) {
        EV_INFO << "5" << endl;
        send(queueOut.dequeue(), "out");
        EV_INFO << "6" << endl;
        if (!queueOut.IsEmpty()) {
            EV_INFO << "7" << endl;
            simtime_t txEnd = getInterfaceDelay();
            cMessage *self = new cMessage("self");
            self->setName(selfString);
            scheduleAt(txEnd > simTime() ? txEnd : simTime(), self);
            EV_INFO << "8" << endl;

        }
        else {
            isSendingMessages = false;
        }
    }
    else
    {
        EV_INFO << "9" << endl;
        TryToSendMessage();
    }
}

simtime_t Analyzer::getInterfaceDelay() {
    simtime_t txFT;
    cChannel *txChn = gate("out")->getTransmissionChannel();
    txFT = txChn->getTransmissionFinishTime();
    return txFT;
}


void Analyzer::DisplayPackagesData()
{
    EV_INFO << "received fresh data" << endl;
    EV_INFO << "dropped: " << droppedMessages << endl;
    EV_INFO << "analyzed: " << analyzedMessages << endl;
    EV_INFO << "AnalyzeMessages: " << startedAnalyzeMessages << endl;
}

void Analyzer::refreshDisplay() const {
    char buf[40];
    sprintf(buf, "rcvd: %lu drpd: %lu", receivedMessages, droppedMessages);
    getDisplayString().setTagArg("t", 0, buf);
}
