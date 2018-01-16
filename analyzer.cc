#include <omnetpp.h>
#include "queue.h"
#include <string>
#include "notification_m.h"
#include "packet_m.h"
#include "global_stats_listener.h"

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
    GlobalStatsListener* globalStats;
};

Define_Module(Analyzer);

void Analyzer::initialize() {
    int usTime = par("analyzeTime");
    analyzeTime = (float)usTime / usInS;
    EV_INFO << "t: " << analyzeTime << endl;

    cModule * mod = getModuleByPath("global_stats");
    if (mod) {
        globalStats = dynamic_cast<GlobalStatsListener*>(mod);
    } else {
        error("No global_stats module.");
    }
}

Analyzer::~Analyzer() {
    Packet* p;
    while ((p = queue.dequeue()) != NULL)
        delete p;
    while ((p = queueOut.dequeue()) != NULL)
           delete p;
}


void Analyzer::handleMessage(cMessage *msg)
{
    if (strncmp(msg->getName(), analyze, 10) == 0)    //if(msg->isSelfMessage())
    {
        delete msg;
        EndAnalyze();
    }
    else if (strncmp(msg->getName(), selfString, 10) == 0)
    {
        delete(msg);
        TryToSendMessage();
    }
    else
    { //receiving from sdn
        ++receivedMessages;
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
            ++droppedMessages;
            ++globalStats->getNumTotalDropped();
            EV_INFO << "dropped packed" << endl;
            delete msg;
        }
    }
}

void Analyzer::StartPackedAnalyze()
{
    isAnalyzing = true;
    EV_INFO << "Started packed analyze" << endl;
    startedAnalyzeMessages++;
    cMessage *delayEvent = new cMessage("delayEvent");
    delayEvent->setName(analyze);
    scheduleAt(simTime() + analyzeTime, delayEvent);
}
void Analyzer::EndAnalyze()
{
    analyzedMessages++;

    if (!isSendingMessages && !queueOut.IsFull())
    {
        isSendingMessages = true;
        queueOut.enqueue(queue.dequeue());
        TryToSendMessage();
    }

    EV_INFO << "ended analyze " << endl;
    if (!queue.IsEmpty())
    {
        StartPackedAnalyze();
    }
    else
        isAnalyzing = false;
}

void Analyzer::TryToSendMessage() {

    simtime_t txEnd = getInterfaceDelay();
    if (txEnd <= simTime()) {
        Packet *pack = check_and_cast<Packet*>(queueOut.dequeue());
        send(pack, "out");
        if (!queueOut.IsEmpty()) {
            simtime_t txEnd = getInterfaceDelay();
            cMessage *self = new cMessage("self");
            self->setName(selfString);
            scheduleAt(txEnd > simTime() ? txEnd : simTime(), self);

        }
        else {
            isSendingMessages = false;
        }
    }
    else
    {
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
