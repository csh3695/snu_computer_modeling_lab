#include <stdio.h>
#include <omnetpp.h>

using namespace omnetpp;

class FFS0Hub: public cSimpleModule
{
    protected:
        virtual void initialize();
        virtual void handleMessage(cMessage *msg);
};

Define_Module(FFS0Hub);

void FFS0Hub::initialize(){
    EV << (double)par("serviceTime0") << endl;
    return;
}

void FFS0Hub::handleMessage(cMessage *msg){
    msg -> setTimestamp(par("serviceTime0"));
    send(msg, "out");
}





class FFSxHub: public cSimpleModule
{
    private:
        double p[4];
    protected:
        virtual void initialize();
        virtual void handleMessage(cMessage *msg);
};

Define_Module(FFSxHub);

void FFSxHub::initialize(){
    p[0] = par("p0");
    p[1] = par("p1");
    p[2] = par("p2");
    p[3] = par("p3");
    EV << p[0] << " " << p[1] << " " << p[2] << " " << p[3] << endl;
    EV << (double)par("serviceTime1") << " " << (double)par("serviceTime2") << " " << (double)par("serviceTime3") << " " << endl;
    return;
}

void FFSxHub::handleMessage(cMessage *msg){
    double prob = uniform(0, 1);
    if(prob < p[0]){
        EV << msg -> getName() << " Leaves" << endl;
        send(msg, "out", 0);
    }
    else if(prob < p[0] + p[1]){
        EV << msg -> getName() << " to s"<< 1 << endl;
        msg -> setTimestamp(par("serviceTime1"));
        send(msg, "out", 1);
    }
    else if(prob < p[0] + p[1] + p[2]){
        EV << msg -> getName() << " to s"<< 2 << endl;
        msg -> setTimestamp(par("serviceTime2"));
        send(msg, "out", 2);
    }
    else{
        EV <<  msg -> getName() << " to s"<< 3 << endl;
        msg -> setTimestamp(par("serviceTime3"));
        send(msg, "out", 3);        
    }

}