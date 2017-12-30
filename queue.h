#ifndef QUEUE_H_
#define QUEUE_H_

#include <omnetpp.h>
#include <packet_m.h>

#define MAX_QUEUE 10000

struct queueElement{
    Packet pack;
    queueElement* next = NULL;
    queueElement(Packet mPacket):pack(mPacket){}
};

class Queue{
private:
    queueElement* listFirst = NULL;
    queueElement* listLast = NULL;
    int length = 0;
public:
    bool enqueue(Packet* pack);
    Packet dequeue();
    int size();
    int getFirstDestination();
};

#endif /* QUEUE_H_ */
