#include <omnetpp.h>
#include "queue.h"
#include <string>
#include "notification_m.h"
#include "packet_m.h"

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
}

void Acx_psl::handleMessage(cMessage *msg){
    std::string type = typeid(*msg).name(); // RTTI, *might* become troublesome due to implementation differences.
    std::size_t found;
    found = type.find("Notification");
    if(singleQueue){
        if(found != std::string::npos){
            //Notification *notif = check_and_cast<Notification*>(msg);
            check_and_cast<Notification*>(msg); // Casting just to check if proper notification was received.
            simtime_t txEnd = getInterfaceDelay(psl.getFirstDestination());
            if(txEnd <= simTime()){
                sendPacket(psl.dequeue());
            }else{
                //Should NOT happen
                EV<<"Unexpected load in outgoing channel - delaying";
                Notification *notif = new Notification();
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
                EV<<"Packet dropped!";
            }
        }
    }else{
        if(found != std::string::npos){
            Notification *notif = check_and_cast<Notification*>(msg);

        }else{
            Packet *notif = check_and_cast<Packet*>(msg);

        }
    }
}
