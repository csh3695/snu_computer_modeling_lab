#include <stdio.h>
#include <omnetpp.h>

using namespace omnetpp;

class FFSink: public cSimpleModule
{
    private:
        cStdDev qstats;
        cOutVector qtime;

    protected:
        virtual void initialize();
        virtual void handleMessage(cMessage *msg);
        virtual void finish();
};

Define_Module(FFSink);

void FFSink::initialize(){
    qstats.setName("queueing time stats");
    qtime.setName("queueing time vector");
}

void FFSink::handleMessage(cMessage *msg){
    simtime_t d = simTime() - msg->getCreationTime();
    EV << "Received" << msg -> getName() << "queueing time: "<< d << "sec" << endl;
    qstats.collect(d);
    qtime.record(d);
    delete msg;
}

void FFSink::finish(){
    EV << "[OVERALL LOG]" << endl;
    EV << "Total jobs processed: " << qstats.getCount() << endl;
    EV << "Avg queueing time:    " << qstats.getMean() << endl;
    EV << "Max queueing time:    " << qstats.getMax() << endl;
    EV << "Standard deviation:   " << qstats.getStddev() << endl;
}