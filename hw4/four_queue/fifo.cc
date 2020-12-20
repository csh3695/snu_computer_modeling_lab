#include <stdio.h>
#include <omnetpp.h>

using namespace omnetpp;

class FFFifo: public cSimpleModule
{
    private:
        cStdDev qstats;
        cOutVector qtime;
        cStdDev Lq;
        cStdDev L;
        cStdDev U;
        cMessage *msgServiced;
        cMessage *endServiceMsg;
        cQueue queue;
        simtime_t prev_timestamp = 0;

    protected:
        virtual void initialize();
        virtual void handleMessage(cMessage *msg);
        void arrival(cMessage *msg);
        virtual simtime_t startService(cMessage *msg);
        virtual void endService(cMessage *msg);
        virtual void finish();
};

Define_Module(FFFifo);


void FFFifo::initialize(){
    msgServiced = nullptr;
    prev_timestamp = 0;
    endServiceMsg = new cMessage("End Service");
}

void FFFifo::handleMessage(cMessage *msg){
    if(msg == endServiceMsg){   // completion
        EV << "Complete " << msgServiced -> getName() << endl;
        endService(msgServiced);
        if(queue.isEmpty()){
            msgServiced = nullptr;
        }
        else{
            msgServiced = (cMessage*)queue.pop();
            simtime_t serviceTime = startService(msgServiced);
            scheduleAt(simTime() + serviceTime, endServiceMsg);
        }
    }
    else if(!msgServiced){  // arrival at idle
        EV << "Arrival at idle" << endl;
        arrival(msg);
        msgServiced = msg;
        simtime_t serviceTime = startService(msg);
        scheduleAt(simTime() + serviceTime, endServiceMsg);
    }
    else{                   // arrival at busy
        EV << "Arrival at busy" << endl;
        arrival(msg);
        queue.insert(msg);
    }
}

simtime_t FFFifo::startService(cMessage *msg){
    EV << "Service Started: time "<< msg -> getTimestamp() << endl;
    return msg -> getTimestamp();
}

void FFFifo::endService(cMessage *msg){
    simtime_t overall_delay = simTime() - (msg -> getArrivalTime());
    qstats.collect(overall_delay);
    qtime.record(overall_delay);

    int l = queue.getLength();
    int u = 0;
    Lq.collect(l*(simTime() - prev_timestamp));
    if (msgServiced) u = 1;
    l += u;
    L.collect(l*(simTime() - prev_timestamp));
    U.collect(u*(simTime() - prev_timestamp));

    prev_timestamp = simTime();

    send(msg, "out");
}

void FFFifo::arrival(cMessage *msg){
    char bbl[20];
    sprintf(bbl, "Queue len %d", queue.getLength());
    bubble(bbl);
    EV << msg -> getName() << " arrived at " << getName() << getIndex() << endl;
    msg -> setArrivalTime(simTime());

    int l = queue.getLength();
    int u = 0;
    Lq.collect(l*(simTime() - prev_timestamp));
    if (msgServiced) u = 1;
    l += u;
    L.collect(l*(simTime() - prev_timestamp));
    U.collect(u*(simTime() - prev_timestamp));

    prev_timestamp = simTime();
}

void FFFifo::finish(){
    EV << "[LOG FOR " << getName() << getIndex() << "]" << endl;
    EV << "Total jobs processed: " << qstats.getCount() << endl;
    EV << "Avg queueing time:    " << qstats.getMean() << endl;
    EV << "Max queueing time:    " << qstats.getMax() << endl;
    EV << "Standard deviation:   " << qstats.getStddev() << endl;
    EV << "L:                    " << L.getSum()/simTime() << endl;
    EV << "Lq:                   " << Lq.getSum()/simTime() << endl;
    EV << "U:                   " << U.getSum()/simTime() << endl;
    EV << endl;
}