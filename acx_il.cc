#include <omnetpp.h>
#include "queue.h"
#include <string>
#include "notification_m.h"
#include "packet_m.h"
#include "global_stats_listener.h"

using namespace omnetpp;

class Acx_il : public cSimpleModule {
private:
    bool singleQueue;
    // For single queue operation
    Queue il;
    // For multi queue operation
    Queue pg;
    Queue pw;
    simtime_t getInterfaceDelay(int destination);
    void sendPacket(Packet *data);
    GlobalStatsListener* globalStats;
protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
};

Define_Module(Acx_il);

simtime_t Acx_il::getInterfaceDelay(int destination){
    simtime_t txFT;
        if(destination == 0){
            cChannel *txChn = gate("il")->getTransmissionChannel();
            txFT = txChn->getTransmissionFinishTime();
        }else{
            if(destination == 1){
                cChannel *txChn = gate("pw$o")->getTransmissionChannel();
                txFT = txChn->getTransmissionFinishTime();
            }else{
                cChannel *txChn = gate("pg$o")->getTransmissionChannel();
                txFT = txChn->getTransmissionFinishTime();
            }
        }
        return txFT;
}

void Acx_il::sendPacket(Packet *data){
    if(data->getDestinationAddress()==0){
            send(data,"il");
        }else{
            if(data->getDestinationAddress()==1){
                send(data,"pw$o");
            }else{
                send(data,"pg$o");
            }
        }
}

void Acx_il::initialize(){
    singleQueue = par("singleQueue");

    cModule * mod = getModuleByPath("total_stats");
    if (mod) {
        globalStats = dynamic_cast<GlobalStatsListener*>(mod);
    } else {
        error("No global_stats module.");
    }
}

void Acx_il::handleMessage(cMessage *msg){
    std::string type = typeid(*msg).name(); // RTTI, *might* become troublesome due to implementation differences.
    std::size_t found;
    found = type.find("Notification");
    if(singleQueue){
        if(found != std::string::npos){
            Notification *notif = check_and_cast<Notification*>(msg);
//            check_and_cast<Notification*>(msg); // Casting just to check if proper notification was received.
            simtime_t txEnd = getInterfaceDelay(il.getFirstDestination());
            if(txEnd <= simTime()){
                sendPacket(il.dequeue());
                delete(notif);
                if(il.size()>0){
                    simtime_t txEnd = getInterfaceDelay(il.getFirstDestination());
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
            bool test = il.enqueue(pack);
            if(test){
                if(il.size() == 1){
                    simtime_t txEnd = getInterfaceDelay(il.getFirstDestination());
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
                simtime_t txEnd = getInterfaceDelay(il.getFirstDestination());
                if(txEnd <= simTime()){
                    sendPacket(il.dequeue());
                    delete(notif);
                    if(il.size()>0){
                        simtime_t txEnd = getInterfaceDelay(il.getFirstDestination());
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
            }
        }else{
            Packet *input = check_and_cast<Packet*>(msg);
            bool test;
            if(input->getDestinationAddress() == 0){
                test=il.enqueue(input);
                if(test){
                    if(il.size() == 1){
                        simtime_t txEnd = getInterfaceDelay(il.getFirstDestination());
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
                delete(input);
                EV<<"This packet should not appear here!";
                ++globalStats->getNumTotalDropped();
            }
        }
    }
}
