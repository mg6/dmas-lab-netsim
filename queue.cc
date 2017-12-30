#include "queue.h"

bool Queue::enqueue(Packet* pack){
    if(Queue::length++<MAX_QUEUE){
        queueElement* qE = new queueElement(pack); // If adding to queue breaks, look for error here
        if(Queue::length == 0){
            Queue::listFirst = qE;
            Queue::listLast = qE;
            Queue::length++;
        }else{
            Queue::listLast->next = qE;
            Queue::listLast = qE;
            Queue::length++;
        }
        return true;
    }else{
        return false;
    }
}

Packet* Queue::dequeue(){
    if(Queue::length>0){
        Packet* pack;
        queueElement* temporary;
        temporary = Queue::listFirst;
        Queue::listFirst = temporary->next;
        Queue::length--;
        pack = temporary->pack;
        delete temporary;
        if(Queue::length == 1){
            Queue::listLast = NULL;
        }
        return pack;
    }else{
        // This case should NOT happen.
        return NULL;
    }
}

int Queue::size(){
    return Queue::length;
}

int Queue::getFirstDestination(){
    return Queue::listFirst->pack->getDestinationAddress();
}
