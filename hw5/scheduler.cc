#include <stdio.h>
#include <omnetpp.h>
#include "message_m.h"

#define BACKGROUND 0
#define POLLING 1
#define DEFERRABLE 2
#define SPORADIC 3

#define APERIODIC 0
#define PERIODIC 1

#define RECHARGE 99

using namespace omnetpp;

class scheduler: public cSimpleModule
{
    private:
        int mode;
        bool bg;
        bool in_budget;

        simtime_t budget;
        simtime_t full_budget;
        simtime_t polling_period;

        cQueue queue[3];
        cQueue sporadicQueue;

        rMessage *msgServiced;
        rMessage *endServiceMsg;
        rMessage *pollingMsg;
        rMessage *budgetEndMsg;
        rMessage *budgetRechargeMsg;

    public:
        scheduler();

    protected:
        virtual void initialize();
        virtual void handleMessage(cMessage *msg);

        void background(rMessage *msg);
        void polling(rMessage *msg);
        void deferrable(rMessage *msg);
        void sporadic(rMessage *msg);

        void arrival(rMessage *msg);
        virtual simtime_t startService(rMessage *msg);
        virtual simtime_t stopService(rMessage *msg);
        virtual void endService(rMessage *msg);

        rMessage* getJob();
        void runJob(rMessage *msg);
};

Define_Module(scheduler);

scheduler::scheduler(){
    in_budget = false;
    polling_period = (simtime_t)2.0;
}

void scheduler::initialize(){
    msgServiced = nullptr;
    endServiceMsg = new rMessage("End Service");
    pollingMsg = new rMessage("Polling");
    budgetEndMsg = new rMessage("Budget End");
    budgetRechargeMsg = new rMessage("Budget Recharge");
    budgetRechargeMsg -> setKind(RECHARGE);

    mode = par("mode").intValue();
    bg = par("background").boolValue();
    full_budget = par("budget").doubleValue();
    WATCH(budget);
    WATCH(msgServiced);
    if(mode == POLLING or mode == DEFERRABLE){
        scheduleAt(simTime(), pollingMsg);
    }
    if(mode == SPORADIC){
        budget = full_budget;
    }
    EV << "mode" << mode << "budget" << budget << "full_budget" << full_budget << endl;
    EV << "bg" << (bg?" true":" false");
}

void scheduler::handleMessage(cMessage *msg){
    switch (mode)
    {
    case BACKGROUND:
        background((rMessage*)msg);
        break;
    case POLLING:
        polling((rMessage*)msg);
        break;
    case DEFERRABLE:
        deferrable((rMessage*)msg);
        break;
    case SPORADIC:
        sporadic((rMessage*)msg);
        break;
    default:
        EV << "WRONG MODE FOR SCHEDULER! GOT MODE "<< mode << endl;
        break;
    }
    char bbl[100];
    sprintf(bbl, "current job %d, in_budget %s", msgServiced?(msgServiced -> getSchedulingPriority()):-1, (in_budget? "true":"false"));
    bubble(bbl);

}

void scheduler::background(rMessage *msg){
    EV << "Background Mode" << endl;
    if(msg == endServiceMsg){
        EV << "Complete " << msgServiced -> getName() << endl;
        endService(msgServiced);
        msgServiced = nullptr;
        runJob(getJob());
    }
    else{
        if(!msgServiced){   // arrival at idle
            if(msg -> getKind() == APERIODIC && !bg){
                queue[msg -> getSchedulingPriority()].insert(msg);
            }
            else runJob(msg);
        }
        else{
            EV << "Arrival at busy" << endl;
            EV << msgServiced -> getSchedulingPriority() << endl;
            if(msgServiced -> getSchedulingPriority() > msg -> getSchedulingPriority()){
                bubble("evicted!");
                stopService(msgServiced);
                runJob(msg);
            }
            else{
                queue[msg -> getSchedulingPriority()].insert(msg);
            }           
        }
    }
}


void scheduler::polling(rMessage *msg){
    if(msg == pollingMsg){
        scheduleAt(simTime() + polling_period, pollingMsg);
        if(msgServiced) stopService(msgServiced);
        if(!queue[2].isEmpty()){
            in_budget = true;
            runJob((rMessage*)queue[2].pop());
            scheduleAt(simTime() + full_budget, budgetEndMsg);
        }
    }
    else if(msg == budgetEndMsg){
        if(msgServiced) stopService(msgServiced);
        in_budget = false;
        runJob(getJob());
    }
    else if(msg == endServiceMsg){
        EV << "Complete " << msgServiced -> getName() << endl;
        endService(msgServiced);
        msgServiced = nullptr;
        if(in_budget && !queue[2].isEmpty()){
            runJob((rMessage*)queue[2].pop());
        }
        else{
            cancelEvent(budgetEndMsg);
            in_budget = false;
            runJob(getJob());
        }
    }
    else{
        if(!msgServiced){   // arrival at idle
            if(msg -> getKind() == APERIODIC && !bg) queue[msg -> getSchedulingPriority()].insert(msg);
            else runJob(msg);
        }
        else{   // arrival at busy
            if(in_budget) queue[msg -> getSchedulingPriority()].insert(msg);
            else{   // periodic is running now: by_priority mode
                if(msgServiced -> getSchedulingPriority() > msg -> getSchedulingPriority()){
                    stopService(msgServiced);
                    runJob(msg);
                }
                else queue[msg -> getSchedulingPriority()].insert(msg);
            }
        }
    }
}


void scheduler::deferrable(rMessage *msg){
    if(msg == pollingMsg){
        cancelEvent(budgetEndMsg);
        scheduleAt(simTime() + polling_period, pollingMsg);
        budget = full_budget;
        if(msgServiced) stopService(msgServiced);
        if(!queue[2].isEmpty()){
            in_budget = true;
            runJob((rMessage*)queue[2].pop());
            scheduleAt(simTime() + full_budget, budgetEndMsg);
        }
    }
    else if(msg == budgetEndMsg){
        budget = 0;        
        if(msgServiced) stopService(msgServiced);
        in_budget = false;
        runJob(getJob());
    }
    else if(msg == endServiceMsg){
        EV << "Complete " << msgServiced -> getName() << endl;
        if(in_budget){
            cancelEvent(budgetEndMsg);
            simtime_t budget_consumed = (simTime() - msgServiced -> getStartTime());
            budget -= budget_consumed;
            in_budget = false;
        }
        endService(msgServiced);
        msgServiced = nullptr;
        if(budget > 0 && !queue[2].isEmpty()){
            scheduleAt(simTime() + budget, budgetEndMsg);
            runJob((rMessage*)queue[2].pop());
            in_budget = true;
        }
        else{   // budget이 남고 periodic job만 남았거나, budget이 없다면 --> priority mode
            runJob(getJob());
        }
    }
    else{
        if(!msgServiced){   // arrival at idle
            if(msg -> getKind() == APERIODIC){
                if(budget > 0){
                    in_budget = true;
                    scheduleAt(simTime() + budget, budgetEndMsg);
                    runJob(msg);
                }
                else{
                    if(bg) runJob(msg);
                    else queue[msg -> getSchedulingPriority()].insert(msg);
                }
            }
            else{
                runJob(msg);
            }
        }
        else{   // arrival at busy
            if(in_budget) queue[msg -> getSchedulingPriority()].insert(msg);
            else{
                if(msg -> getKind() == APERIODIC){
                    if(budget > 0){
                        stopService(msgServiced);
                        in_budget = true;
                        scheduleAt(simTime() + budget, budgetEndMsg);
                        runJob(msg);
                    }
                    else{
                        queue[msg -> getSchedulingPriority()].insert(msg);
                    }
                }
                else{
                    if(msgServiced -> getSchedulingPriority() > msg -> getSchedulingPriority()){
                        stopService(msgServiced);
                        runJob(msg);
                    }
                    else{
                        queue[msg -> getSchedulingPriority()].insert(msg);
                    }
                }
            }
        }
    }
}


void scheduler::sporadic(rMessage *msg){
    if(in_budget){
        /* budget 깎인 만큼만 뺄 수 없을까?*/
        budget -= (simtime_t)(simTime() - msgServiced -> getTmpStartTime());
        msgServiced -> setTmpStartTime(simTime());
    }
    if(msg -> getKind() == RECHARGE){
        cancelEvent(budgetEndMsg);
        rMessage* increase = (rMessage*)sporadicQueue.pop();
        //EV << "BUDGET RECHARGED from " << budget << " to ";
        budget += (simtime_t)(increase -> getTimestamp());
        EV << "[+]" << (simtime_t)(increase -> getTimestamp()) << " :" << budget;
        //EV << budget << endl;
        delete increase;
        if(!queue[2].isEmpty() && msgServiced -> getKind() != APERIODIC){
            if(msgServiced) stopService(msgServiced);
            in_budget = true;
            scheduleAt(simTime() + polling_period, budgetRechargeMsg -> dup());
            runJob((rMessage*)queue[2].pop());
        }
        if(in_budget){
            scheduleAt(simTime() + budget, budgetEndMsg);
            EV << "budgetEndMsg at " << simTime() + budget << endl;
        }
        delete msg;
    }
    else if(msg == budgetEndMsg){
        budget = 0;
        if(msgServiced){
            if(msgServiced -> getKind() == APERIODIC){
                rMessage *decreased = new rMessage("decreased");
                EV << "[-be]" << simTime() - msgServiced -> getStartTime() << " :" << budget;
                decreased -> setTimestamp(stopService(msgServiced));
                sporadicQueue.insert(decreased);
            }
        }
        in_budget = false;
        runJob(getJob());
    }
    else if(msg == endServiceMsg){
        EV << simTime() << " Complete " << msgServiced -> getName() <<" " << msgServiced -> getId() << endl;
        if(in_budget){
            cancelEvent(budgetEndMsg);
            simtime_t budget_consumed = (simTime() - msgServiced -> getStartTime());
            //budget -= budget_consumed;
            EV << "[-se]" << budget_consumed << " :" << budget;
            in_budget = false;

            rMessage *decreased = new rMessage("decreased");
            decreased -> setTimestamp(budget_consumed);
            sporadicQueue.insert(decreased);
        }
        endService(msgServiced);
        msgServiced = nullptr;
        if(budget > 0 && !queue[2].isEmpty()){
            in_budget = true;
            scheduleAt(simTime() + polling_period, budgetRechargeMsg -> dup());
            scheduleAt(simTime() + budget, budgetEndMsg);
            EV << "budgetEndMsg at " << simTime() + budget << endl;
            runJob((rMessage*)queue[2].pop());
        }
        else{   // budget이 남고 periodic job만 남았거나, budget이 없다면 --> priority mode
            runJob(getJob());
        }
    }
    else{
        if(!msgServiced){   // arrival at idle
            if(msg -> getKind() == APERIODIC){
                if(budget > 0){ // updated budget
                    in_budget = true;
                    scheduleAt(simTime() + polling_period, budgetRechargeMsg -> dup());
                    scheduleAt(simTime() + budget, budgetEndMsg);
                    EV << "budgetEndMsg at " << simTime() + budget << endl;
                    runJob(msg);
                }
                else{
                    if(bg) runJob(msg);
                    else queue[msg -> getSchedulingPriority()].insert(msg);
                }
            }
            else{
                runJob(msg);
            }
        }
        else{   // arrival at busy
            if(in_budget) queue[msg -> getSchedulingPriority()].insert(msg);
            else{
                if(msg -> getKind() == APERIODIC){
                    if(budget > 0){ // updated budget
                        stopService(msgServiced);
                        in_budget = true;
                        scheduleAt(simTime() + polling_period, budgetRechargeMsg -> dup());
                        scheduleAt(simTime() + budget, budgetEndMsg);
                        EV << "budgetEndMsg at " << simTime() + budget << endl;
                        runJob(msg);
                    }
                    else{
                        queue[msg -> getSchedulingPriority()].insert(msg);
                    }
                }
                else{
                    if(msgServiced -> getSchedulingPriority() > msg -> getSchedulingPriority()){
                        stopService(msgServiced);
                        runJob(msg);
                    }
                    else{
                        queue[msg -> getSchedulingPriority()].insert(msg);
                    }
                }
            }
        }
    }
}


simtime_t scheduler::stopService(rMessage *msg){
    cancelEvent(endServiceMsg);
    simtime_t doneby = simTime() - msg -> getStartTime();
    //assert(msg -> getTimestamp() - doneby >= 0);
    msg -> setTimestamp(msg -> getTimestamp() - doneby);
    if(queue[msg -> getSchedulingPriority()].front()){
        queue[msg -> getSchedulingPriority()].insertBefore(queue[msg -> getSchedulingPriority()].front(), msg);
    }
    else{
        queue[msg -> getSchedulingPriority()].insert(msg);
    }
    msgServiced = nullptr;
    return doneby;
}

simtime_t scheduler::startService(rMessage *msg){
    msg -> setStartTime(simTime());
    msg -> setTmpStartTime(simTime());
    return msg -> getTimestamp();
}

void scheduler::endService(rMessage *msg){
    send(msg, "out");
}

rMessage* scheduler::getJob(){
    for(int i=0; i<(bg?3:2); i++){ // 다른 job에 넘겨 주기
        if(!queue[i].isEmpty()){
            return (rMessage*)queue[i].pop();
        }
    }
    return nullptr;
}

void scheduler::runJob(rMessage* msg){
    if(msg){
        msgServiced = msg;
        EV << endl << simTime() << " Start " << msgServiced -> getName() <<" " << msgServiced -> getId() << endl;
        simtime_t serviceTime = startService(msgServiced);
        scheduleAt(simTime() + serviceTime, endServiceMsg);
    }
}