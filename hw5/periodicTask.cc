#include <stdio.h>
#include <omnetpp.h>
#include "message_m.h"

using namespace omnetpp;

class periodicTask: public cSimpleModule
{
    private:
        rMessage *sendMessageEvent;
        int job_id;
        short priority;

    public:
        periodicTask();
        virtual ~periodicTask();

    protected:
        virtual void initialize();
        virtual void handleMessage(cMessage *msg);
        virtual void cancelAndDelete(cMessage *msg);
};

Define_Module(periodicTask);

periodicTask::periodicTask(){
    sendMessageEvent = nullptr;
}

periodicTask::~periodicTask(){
    cancelAndDelete(sendMessageEvent);
}


void periodicTask::initialize(){
    job_id = 0;
    priority = (short)par("priority").intValue();
    sendMessageEvent = new rMessage("sendMessageEvent");
    scheduleAt(simTime(), sendMessageEvent);
}

void periodicTask::handleMessage(cMessage *msg){
    ASSERT(msg == sendMessageEvent);
    char id[20];
    sprintf(id, "%s job %d", getName(), job_id);
    job_id++;
    rMessage *m = new rMessage(id);
    m -> setKind(1);
    m -> setSchedulingPriority(priority);
    m -> setTimestamp(par("execTime"));
    send(m, "out");

    scheduleAt(simTime() + par("period").doubleValue(), sendMessageEvent);
}

void periodicTask::cancelAndDelete(cMessage *msg){
    cancelEvent(msg);
    delete msg;
    msg = nullptr;
}