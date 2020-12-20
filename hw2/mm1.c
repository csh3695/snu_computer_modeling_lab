//========================================================== file = mm1.c =====
//=  A simple M/M/1 queue simulation using SMPL                               =
//=============================================================================
//=  Notes: This program is adapted from "Figure 1.7" in Simulating Computer  =
//=         Systems, Techniques and Tools by M. H. MacDougall (1987)          =
//=---------------------------------------------------------------------------=
//=  Build: bcc32 mm1.c smpl.c bmeans.c rand.c                                =
//=         gcc mm1.c smpl.c bmeans.c rand.c -lm                              =
//=---------------------------------------------------------------------------=
//=  History: KJC (10/04/98) - Genesis                                        =
//=           KJC (02/18/05) - Cleaned-up for better understanding            =
//=============================================================================
//----- Include files ---------------------------------------------------------
#include <stdio.h>      // Needed for printf()
#include "smpl.h"       // Needed for SMPL

//===== Main program ==========================================================

void main(void)
{
  int sim_len = 10;
  double max_lambda = 0.95;
  double lambda_range[] = {2.0, 1.95, 1.9, 1.5, 1.0, 0.95, 0.76, 0.57, 0.38, 0.19};
  
  //for(int i=0; i<sim_len; i++) lambda_range[i] = max_lambda * ((double)(i+1)/(double)(sim_len));

  real Ta = 1;        // Mean interarrival time (seconds)
  real Ts = 1;        // Mean service time
  real te = 1.0e5;      // Total simulation time
  int customer = 1;     // Customer id (always '1' for this simulation)
  int event;            // Event (1 = arrival, 2 = request, 3 = completion)
  int server;           // Handle for server facility

  printf("lambda mu traffic_load total_time waiting_by_job njob_avg nq_avg\n");  
  for(int i=0; i<sim_len; i++){
    Ta = (1/lambda_range[sim_len-1-i])*Ts;
    // Initialize SMPL subsystem
    smpl(0, "M/M/1 Queue");

    // Initialize server facility (single server)
    server=facility("server", 2);

    // Schedule arrival event at time 0 to kick-off simulation
    schedule(1, 0.0, customer);

    // Loop while simulation time is less than te
    while (time() < te)
    {
      // "Cause" the next event on the event list
      cause(&event,&customer);

      // Process the event
      switch(event)
      {
        case 1:  // *** Arrival
          schedule(2, 0.0, customer);
          schedule(1, expntl(Ta), customer);
          break;

        case 2:  // *** Request Server
          if (request(server, customer, 0) == 0)
            schedule(3, expntl(Ts), customer);
          break;

        case 3:  // *** Release server
          release(server, customer);
          break;
      }
    }
    // Output standard SMPL report
    printf("%f %f %f %f %f %f %f\n", 1/Ta, 1/Ts, Ts/Ta, time(), (Lq(server) + U(server))*Ta, Lq(server) + U(server), Lq(server));
    //report();
  }
}
