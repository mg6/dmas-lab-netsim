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
    double getInterfaceDelay(int destination);
protected:
    virtual void initialize() override;
    virtual void handleMessage(cMessage *msg) override;
};

Define_Module(Acx_psl);

double Acx_psl::getInterfaceDelay(int destination){

}

void Acx_psl::initialize(){
    singleQueue = par("singleQueue");
//    Notification *notif = new Notification();
//    scheduleAt(simTime() + 30, notif);
}

void Acx_psl::handleMessage(cMessage *msg){
    std::string type = typeid(*msg).name(); // RTTI, *might* become troublesome due to implementation differences.
    std::size_t found;
    found = type.find("Notification");
    if(singleQueue){
        if(found != std::string::npos){
            Notification *notif = check_and_cast<Notification*>(msg);

        }else{
            Packet *pack = check_and_cast<Packet*>(msg);
            bool test = psl.enqueue(pack);
            if(test){
                if(psl.size() == 1){
                    double delay = getInterfaceDelay(psl.getFirstDestination());
                    Notification *notif = new Notification();
                    scheduleAt(delay, notif);
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
