#include <stdio.h>
#include <omnetpp.h>

using namespace omnetpp;

class FFGenerator: public cSimpleModule
{
    private:
        cMessage *sendMessageEvent;
        int job_id;

    public:
        FFGenerator();
        virtual ~FFGenerator();

    protected:
        virtual void initialize();
        virtual void handleMessage(cMessage *msg);
        virtual void cancelAndDelete(cMessage *msg);
};

Define_Module(FFGenerator);

FFGenerator::FFGenerator(){
    sendMessageEvent = nullptr;
}

FFGenerator::~FFGenerator(){
    cancelAndDelete(sendMessageEvent);
}


void FFGenerator::initialize(){
    job_id = 0;
    sendMessageEvent = new cMessage("sendMessageEvent");
    scheduleAt(simTime(), sendMessageEvent);
}

void FFGenerator::handleMessage(cMessage *msg){
    ASSERT(msg == sendMessageEvent);
    char id[20];
    sprintf(id, "job %d", job_id);
    job_id++;
    cMessage *m = new cMessage(id);
    send(m, "out");

    scheduleAt(simTime() + par("sendIaTime").doubleValue(), sendMessageEvent);
}

void FFGenerator::cancelAndDelete(cMessage *msg){
    cancelEvent(msg);
    delete msg;
    msg = nullptr;
}