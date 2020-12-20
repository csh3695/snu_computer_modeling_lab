#include <stdio.h>
#include <omnetpp.h>
#include "message_m.h"

#define APERIODIC 0
#define PERIODIC 1

using namespace omnetpp;

class sink: public cSimpleModule
{
    private:
        cStdDev qstats;
        cOutVector qtime;

    protected:
        virtual void initialize();
        virtual void handleMessage(cMessage *msg);
        virtual void finish();
};

Define_Module(sink);

void sink::initialize(){
    qstats.setName("queueing time stats");
    qtime.setName("queueing time vector");
}

void sink::handleMessage(cMessage *msg){
    if(msg -> getKind() == APERIODIC){
        simtime_t d = simTime() - msg->getCreationTime();
        EV << "Received aperiodic " << msg -> getName() << " queueing time: "<< d << "sec" << endl;
        qstats.collect(d);
        qtime.record(d);
    }
    else{
        simtime_t d = simTime() - msg->getCreationTime();
        EV << "Received periodic " << msg -> getName() << " queueing time: "<< d << "sec" << endl;
    }
    delete msg;
    msg = nullptr;
}

void sink::finish(){
    EV << "[OVERALL LOG]" << endl;
    EV << "Total jobs processed: " << qstats.getCount() << endl;
    EV << "Avg queueing time:    " << qstats.getMean() << endl;
    EV << "Max queueing time:    " << qstats.getMax() << endl;
    EV << "Standard deviation:   " << qstats.getStddev() << endl;
}