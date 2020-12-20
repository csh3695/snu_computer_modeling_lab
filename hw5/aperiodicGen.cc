#include <stdio.h>
#include <omnetpp.h>
#include "message_m.h"

using namespace omnetpp;

class aperiodicGen: public cSimpleModule
{
    private:
        rMessage *sendMessageEvent;
        int job_id;
        short priority;

    public:
        aperiodicGen();
        virtual ~aperiodicGen();

    protected:
        virtual void initialize();
        virtual void handleMessage(cMessage *msg);
        virtual void cancelAndDelete(cMessage *msg);
};

Define_Module(aperiodicGen);

aperiodicGen::aperiodicGen(){
    sendMessageEvent = nullptr;
}

aperiodicGen::~aperiodicGen(){
    cancelAndDelete(sendMessageEvent);
}


void aperiodicGen::initialize(){
    job_id = 0;
    priority = (short)par("priority").intValue();
    sendMessageEvent = new rMessage("sendMessageEvent");
    scheduleAt(simTime(), sendMessageEvent);
}

void aperiodicGen::handleMessage(cMessage *msg){
    ASSERT(msg == sendMessageEvent);
    char id[20];
    sprintf(id, "%s job %d", getName(), job_id);
    job_id++;
    rMessage *m = new rMessage(id);
    m -> setKind(0);
    m -> setSchedulingPriority(priority);
    m -> setTimestamp(par("execTime"));
    send(m, "out");

    scheduleAt(simTime() + par("sendIaTime").doubleValue(), sendMessageEvent);
}

void aperiodicGen::cancelAndDelete(cMessage *msg){
    cancelEvent(msg);
    delete msg;
    msg = nullptr;
}