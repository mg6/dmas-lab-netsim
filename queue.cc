#include "queue.h"

bool Queue::enqueue(Packet* pack){
    if(length<(MAX_QUEUE+1)){
        queueElement* qE = new queueElement(pack); // If adding to queue breaks, look for error here
        if(length == 0){
            listFirst = qE;
            listLast = qE;
            length++;
        }else{
            listLast->next = qE;
            listLast = qE;
            length++;
        }
        return true;
    }else{
        return false;
    }
}

Packet* Queue::dequeue(){
    if(length>0){
        Packet* pack;
        queueElement* temporary;
        temporary = Queue::listFirst;
        listFirst = temporary->next;
        length--;
        pack = temporary->pack;
        delete temporary;
        if(length == 1){
            listLast = NULL;
        }
        return pack;
    }else{
        // This case should NOT happen.
        return NULL;
    }
}

int Queue::size(){
    return length;
}

bool Queue::IsFull(){
    return length>=MAX_QUEUE;
}
bool Queue::IsEmpty(){
    return length==0;
}

int Queue::getFirstDestination(){
    return listFirst->pack->getDestinationAddress();
}
