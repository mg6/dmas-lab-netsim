#include <omnetpp.h>
#include "queue.h"
#include <string>
#include "notification_m.h"
#include "packet_m.h"
#include "global_stats_listener.h"

using namespace omnetpp;

class Acx_psl : public cSimpleModule {
private:
    bool singleQueue;
    // For single queue operation
    Queue psl;
    // For multi queue operation
    Queue pwr;
    Queue pw;
    simtime_t getInterfaceDelay(int destination);
    void sendPacket(Packet *data);
    GlobalStatsListener* globalStats;
protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
};

Define_Module(Acx_psl);

simtime_t Acx_psl::getInterfaceDelay(int destination){
    simtime_t txFT;
    if(destination < 2){
        cChannel *txChn = gate("pw$o")->getTransmissionChannel();
        txFT = txChn->getTransmissionFinishTime();
    }else{
        cChannel *txChn = gate("pwr$o")->getTransmissionChannel();
        txFT = txChn->getTransmissionFinishTime();
    }
    return txFT;
}

void Acx_psl::sendPacket(Packet *data){
    if(data->getDestinationAddress()<2){
        send(data,"pw$o");
    }else{
        send(data,"pwr$o");
    }
}

void Acx_psl::initialize(){
    singleQueue = par("singleQueue");

    cModule * mod = getModuleByPath("global_stats");
    if (mod) {
        globalStats = dynamic_cast<GlobalStatsListener*>(mod);
    } else {
        error("No global_stats module.");
    }
}

void Acx_psl::handleMessage(cMessage *msg){
    std::string type = typeid(*msg).name(); // RTTI, *might* become troublesome due to implementation differences.
    std::size_t found;
    found = type.find("Notification");
    if(singleQueue){
        if(found != std::string::npos){
            Notification *notif = check_and_cast<Notification*>(msg);
//            check_and_cast<Notification*>(msg); // Casting just to check if proper notification was received.
            simtime_t txEnd = getInterfaceDelay(psl.getFirstDestination());
            if(txEnd <= simTime()){
                sendPacket(psl.dequeue());
                delete(notif);
                if(psl.size()>0){
                    simtime_t txEnd = getInterfaceDelay(psl.getFirstDestination());
                    Notification *notif = new Notification();
                    scheduleAt(txEnd>simTime()?txEnd:simTime(), notif);
                }
            }else{
                //Should NOT happen
                EV<<"Unexpected load in outgoing channel - delaying";
//                Notification *notif = new Notification();
                scheduleAt(txEnd, notif);
            }
        }else{
            Packet *pack = check_and_cast<Packet*>(msg);
            bool test = psl.enqueue(pack);
            if(test){
                if(psl.size() == 1){
                    simtime_t txEnd = getInterfaceDelay(psl.getFirstDestination());
                    Notification *notif = new Notification();
                    scheduleAt(txEnd>simTime()?txEnd:simTime(), notif);
                }
            }else{
                delete(pack);
                EV<<"Packet dropped!";
                ++globalStats->getNumTotalDropped();
            }
        }
    }else{
        if(found != std::string::npos){
            Notification *notif = check_and_cast<Notification*>(msg);
            if(notif->getQueue() == 0){
                simtime_t txEnd = getInterfaceDelay(pw.getFirstDestination());
                if(txEnd <= simTime()){
                    sendPacket(pw.dequeue());
                    delete(notif);
                    if(pw.size()>0){
                        simtime_t txEnd = getInterfaceDelay(pw.getFirstDestination());
                        Notification *notif = new Notification();
                        notif->setQueue(0);
                        scheduleAt(txEnd>simTime()?txEnd:simTime(), notif);
                    }
                }else{
                    //Should NOT happen
                    EV<<"Unexpected load in outgoing channel - delaying";
//                    Notification *notif = new Notification();
//                    notif->setQueue(0);
                    scheduleAt(txEnd, notif);
                }
            }else{
                simtime_t txEnd = getInterfaceDelay(pwr.getFirstDestination());
                if(txEnd <= simTime()){
                    sendPacket(pwr.dequeue());
                    delete(notif);
                    if(pwr.size()>0){
                        simtime_t txEnd = getInterfaceDelay(pwr.getFirstDestination());
                        Notification *notif = new Notification();
                        notif->setQueue(1);
                        scheduleAt(txEnd>simTime()?txEnd:simTime(), notif);
                    }
                }else{
                    //Should NOT happen
                    EV<<"Unexpected load in outgoing channel - delaying";
//                    Notification *notif = new Notification();
//                    notif->setQueue(1);
                    scheduleAt(txEnd, notif);
                }
            }
        }else{
            Packet *input = check_and_cast<Packet*>(msg);
            bool test;
            if(input->getDestinationAddress() < 2){
                test=pw.enqueue(input);
                if(test){
                    if(pw.size() == 1){
                        simtime_t txEnd = getInterfaceDelay(pw.getFirstDestination());
                        Notification *notif = new Notification();
                        notif->setQueue(0);
                        scheduleAt(txEnd>simTime()?txEnd:simTime(), notif);
                    }
                }else{
                    delete(input);
                    EV<<"Packet dropped!";
                    ++globalStats->getNumTotalDropped();
                }
            }else{
                test=pwr.enqueue(input);
                if(test){
                    if(pwr.size() == 1){
                        simtime_t txEnd = getInterfaceDelay(pwr.getFirstDestination());
                        Notification *notif = new Notification();
                        notif->setQueue(1);
                        scheduleAt(txEnd>simTime()?txEnd:simTime(), notif);
                    }
                }else{
                    delete(input);
                    EV<<"Packet dropped!";
                    ++globalStats->getNumTotalDropped();
                }
            }
        }
    }
}
