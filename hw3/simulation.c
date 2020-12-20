#include <stdio.h>      // Needed for printf()
#include "smpl.h"       // Needed for SMPL
#define MAX_T 1000000
#define MAX_L 1000000
#define EXTERNAL_ARRIVAL 1
#define INTERNAL_ARRIVAL 2
#define REQUEST 3
#define COMPLETION 4


typedef struct {
    int fid;    // -1 for global, 0 1 2 3 for local
    real duration;
} logentry;

typedef struct {
    int fid;
    real global_start;
    real local_start;
} token;

token tokenPool[MAX_T];
logentry loglist[MAX_L];
int log_id = 0;

void initLog(){
    for(int i=0; i<MAX_L; i++){
        loglist[i].fid = 0x7fffffff;
    }
    log_id = 0;
}

void initTokenPool(){
    for(int i=0; i<MAX_T; i++) tokenPool[i].fid = -1;
}

int getFid(int tid){
    return tokenPool[tid].fid;
}

void setLocalStart(int tid){
    tokenPool[tid].local_start = time();
}

void setGlobalStart(int tid){
    tokenPool[tid].global_start = time();
}

int getToken(int fid){
    for(int i=0; i<MAX_T; i++){
        if(tokenPool[i].fid == -1){
            tokenPool[i].fid = fid;
            return i;
        }
    }
    printf("No Token Available");
    return -1;
}

void writeGlobalTime(int tid){
    // write global Time log
    loglist[log_id].fid = -1;
    loglist[log_id].duration = time() - tokenPool[tid].global_start;
    log_id ++;
}

int writeLocalTime(int tid, int nextFid){
    // write local Time log
    loglist[log_id].fid = tokenPool[tid].fid;
    loglist[log_id].duration = time() - tokenPool[tid].local_start;
    log_id ++;
    tokenPool[tid].fid = nextFid;
    return tid;
}

real freeToken(int tid){
    real duration = time() - tokenPool[tid].local_start;
    tokenPool[tid].fid = -1;
    tokenPool[tid].local_start = 0.0;
    tokenPool[tid].global_start = 0.0;
    return duration;
}

void reporting(real lambda, int* server){
    real Lglobal = 0;
    real Wglobal = 0;
    real L[4] = {0.0, 0.0, 0.0, 0.0};
    real W[4] = {0.0, 0.0, 0.0, 0.0};
    int log_counter[5] = {0, 0, 0, 0, 0}; 
    for(int i=0; i<4; i++){
        L[i] = Lq(server[i]) + U(server[i]);
        Lglobal += L[i];
    }
    for(int i=0; i<log_id; i++){
        int fid = loglist[i].fid;
        real duration = loglist[i].duration;
        if(fid == -1){
            log_counter[4] += 1;
            Wglobal += duration;
        }
        else{
            log_counter[fid] += 1;
            W[fid] += duration;
        }
    }
    Wglobal /= (real)log_counter[4];
    for(int i=0; i<4; i++) W[i] /= (real)log_counter[i];
    printf("%f %f %f %f %f %f %f %f %f %f %f %f %f %f %f\n", lambda, Lglobal, Wglobal, U(server[0]), U(server[1]), U(server[2]), U(server[3]), L[0], W[0], L[1], W[1], L[2], W[2], L[3], W[3]);
}

void main(void)
{
  int sim_len = 101;
  double max_lambda = 1;
  double min_lambda = 0.05;
  double lambda_range[sim_len];
  
  for(int i=0; i<sim_len; i++) lambda_range[i] = (min_lambda + (max_lambda - min_lambda) * ((double)(i)/(double)(sim_len-1)));
  real Ta;        // Mean interarrival time (seconds) = 1/lambda
  real Ts[] = {0.75, 1, 2, 4};        // Mean service time = 1/mu
  real te = 1.0e5;      // Total simulation time
  int tid;
  int fid;
  int event;            // Event (1 = external arrival, 2 = internal arrival, 3 = request, 4 = completion)
  int server[4];           // Handle for server facility

    printf("lambda L W U0 U1 U2 U3 L0 W0 L1 W1 L2 W2 L3 W3\n");

  for(int i=0; i<sim_len; i++){
    initLog();
    initTokenPool();
    
    Ta = (1/lambda_range[i]);
    // Initialize SMPL subsystem
    smpl(0, "HW3 Queueing System");

    // Initialize server facility (single server)
    server[0] = facility("s0", 1);
    server[1] = facility("s1", 1);
    server[2] = facility("s2", 1);
    server[3] = facility("s3", 1);
    // Schedule arrival event at time 0 to kick-off simulation
    tid = getToken(0);
    schedule(EXTERNAL_ARRIVAL, 0.0, tid);

    // Loop while simulation time is less than te
    while (time() < te)
    {
      // "Cause" the next event on the event list
      cause(&event, &tid);
      // Process the event
      switch(event)
      {
        case EXTERNAL_ARRIVAL:
            setLocalStart(tid);
            setGlobalStart(tid);
            schedule(REQUEST, 0.0, tid);
            schedule(EXTERNAL_ARRIVAL, expntl(Ta), getToken(0));
            break;

        case INTERNAL_ARRIVAL:  // *** Arrival
            setLocalStart(tid);
            schedule(REQUEST, 0.0, tid);
            break;

        case REQUEST:  // *** Request Server
            fid = getFid(tid);
            if (request(server[fid], tid, 0) == 0)
                schedule(COMPLETION, expntl(Ts[fid]), tid);
            break;

        case COMPLETION:  // *** Release server
            fid = getFid(tid);
            release(server[fid], tid);
            if(fid == 0){
                int nextfid = (int)uniform(0, 4);
                if(nextfid > 0){
                    tid = writeLocalTime(tid, nextfid);
                    schedule(INTERNAL_ARRIVAL, 0.0, tid);
                }
                else{
                    writeGlobalTime(tid);
                    freeToken(tid);
                }
            }
            else{
                tid = writeLocalTime(tid, 0);
                schedule(INTERNAL_ARRIVAL, 0.0, tid);
            }
            break;
      }
    }
    reporting(1/Ta, server);
    //report();
  }
}

