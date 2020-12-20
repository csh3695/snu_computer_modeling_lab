#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <queue>
#include <ctime>
#include <cassert>
#include <process.h>

#define NULL_E      -1
#define ARRIVAL     0
#define COMPLETION  1
#define IDLE        0
#define BUSY        1
#define MAX_JOB     20000
#define MAX_LOG     54321
#define JOB_LOG     0
#define QUEUE_LOG   1

using namespace std;

class Event{
    public:
    int type;
    double time;
    int job_id;
    Event(int type, double time, int job_id){
        this -> type = type;
        this -> time = time;
        this -> job_id = job_id;
    }
};

class Job{
    public:
    int id;
    double time;
    Job(int id, double time){
        this -> id = id;
        this -> time = time;
    }
};

struct compare{
    bool operator()(Event &e1, Event &e2){
        return e1.time > e2.time;
    }
};

class Simulator{
   
    private:
    
    priority_queue<Event, vector<Event>, compare> event_list;
    queue<Job> q;
    double ranf(){ return (double)rand()/((double)RAND_MAX);}
    double expntl(double x){
        return -x*log(ranf() + 1e-7);
    }
    double lambda, mu, traffic_load;
    double curtime;
    double job_log[MAX_JOB][2] = {};
    double log_timestamp[MAX_LOG] = {};
    int log_njob_nq[MAX_LOG][2] = {};
    int server_state;

    public:
    
    Simulator(double lambda, double mu){
        srand((unsigned int)time(0));
        this -> lambda = lambda;
        this -> mu = mu;
        server_state = IDLE;
        traffic_load = (double)lambda / (double)mu;
    }

    void schedule_event(double time, int type, int job_id){
        event_list.push(Event(type, time, job_id));
    }

    Event get_event(){
        if(event_list.empty())
            return Event(NULL_E, 0, -1);
        Event event = event_list.top();
        event_list.pop();
        return event;
    }

    void print_job_log(){
        int i=0;
        while(job_log[i][1] != 0){
            printf("%d %f %f\n", i, job_log[i][0], job_log[i][1]);
            i++;
        }
    }

    void print_log(){
        int i=0;
        while((i == 0) || (log_timestamp[i] !=0)){
            printf("Time %f njob %d qsize %d\n", log_timestamp[i], log_njob_nq[i][JOB_LOG], log_njob_nq[i][QUEUE_LOG]);
            i++;
        }      
    }

    double* calc_result(){
        int job_i = 0;
        double waiting_by_job = 0.0;
        while(job_log[job_i][1] != 0){
            double diff = (job_log[job_i][1] - job_log[job_i][0]);
            assert(diff >= 0);
            waiting_by_job += diff;
            if(diff < 0){
                printf("%f\n", diff);
                exit(0);
            } 
            job_i++;
        }       
        waiting_by_job /= (double)job_i;

        int time_i = 0;
        double njob_avg = 0;
        double nq_avg = 0;
        double total_time;

        while(true){
            if(log_timestamp[time_i + 1] == 0){
                total_time = log_timestamp[time_i];                
                break;
            }
            double job_incre = abs(log_timestamp[time_i + 1] - log_timestamp[time_i])*((double)log_njob_nq[time_i][JOB_LOG]);
            double q_incre = abs(log_timestamp[time_i + 1] - log_timestamp[time_i])*((double)log_njob_nq[time_i][QUEUE_LOG]);

            njob_avg += job_incre;
            nq_avg += q_incre;
            time_i++;
        }

        njob_avg /= total_time;
        nq_avg /= total_time;

        double* result = (double*) malloc(7*sizeof(double));
        result[0] = lambda;
        result[1] = mu;
        result[2] = traffic_load;
        result[3] = total_time;
        result[4] = waiting_by_job;
        result[5] = njob_avg;
        result[6] = nq_avg;
        return result;
    }

    void log_to_file(string filename){
        string filePath = filename;
        ofstream writeFile(filePath.data());
        writeFile << "time njob nq\n";
        int i=0;
        while((i == 0) || (log_timestamp[i] !=0)){
            writeFile << log_timestamp[i] << " "
                        << log_njob_nq[i][JOB_LOG] << " "
                        << log_njob_nq[i][QUEUE_LOG] << '\n';
            i++;
        }      

        writeFile.close();
    }

    void write_log(int &log_id){
        assert(log_id < MAX_LOG);
        assert(log_timestamp[log_id] == 0);
        log_timestamp[log_id] = curtime;
        log_njob_nq[log_id][JOB_LOG] = q.size() + server_state;
        log_njob_nq[log_id][QUEUE_LOG] = q.size();
        log_id ++;
    }

    double* simulate(double time, bool verbose){
        if(verbose){
            printf("### INFO ###\n");
            printf("  mu=%f\n",mu);
            printf("  lambda=%f\n",lambda);
            printf("  load=%f\n", traffic_load);
            printf("  time=%f\n", time);
        }
        curtime = 0.0;
        int job_id = 0;
        int log_id = 0;
        schedule_event(curtime, ARRIVAL, job_id);
        server_state = IDLE;

        assert(curtime == 0);
        for(int i=0; i<10; i++){
            assert(job_log[i][0] == 0);
            assert(job_log[i][1] == 0);
            assert(log_timestamp[i] == 0);
            assert(log_njob_nq[i][0] == 0);
            assert(log_njob_nq[i][1] == 0);
        }
        assert(job_id == 0);
        assert(log_id == 0);

        while(curtime < time){
            if(event_list.empty()){
                printf("No event to process");
                return NULL;
            }

            Event event = get_event();
            curtime = event.time;   // Event starts

            switch (event.type){
                
                case ARRIVAL:
                {
                    job_log[event.job_id][ARRIVAL] = curtime;

                    if(verbose) printf("%d ARRIVALS | time %f | server IDLE = %d |qsize %d\n", event.job_id, event.time, server_state, q.size());
                    double arrival_time = abs(expntl((double)1.0 / (double)(this -> lambda)));
                    double service_time = abs(expntl((double)1.0 / (double)(this -> mu)));
                    job_id ++;
                    schedule_event(curtime + arrival_time, ARRIVAL, job_id);    // Schedule next arrival
                    if(server_state == BUSY){
                        q.push(Job(event.job_id, service_time));           // Queueing job
                        write_log(log_id);
                    }
                    else if(server_state == IDLE){
                        server_state = BUSY;            // Enters service
                        schedule_event(curtime + service_time, COMPLETION, event.job_id); // Schedule completion of this event
                        write_log(log_id);
                    }
                    break;
                }

                case COMPLETION:
                {
                    job_log[event.job_id][COMPLETION] = curtime;
                    assert(job_log[event.job_id][COMPLETION] >= job_log[event.job_id][ARRIVAL]);

                    if(verbose) printf("%d COMPLETE | time %f | server IDLE = %d |qsize %d\n", event.job_id, event.time, server_state, q.size());
                    if(q.empty()){
                        server_state = IDLE;        // Nothing to do
                        write_log(log_id);
                    }
                    else{
                        Job job = q.front();
                        q.pop();
                        double service_time = job.time;    // Dequeueing job
                        server_state = BUSY;                // Enters service
                        schedule_event(curtime + service_time, COMPLETION, job.id); // Schedule completion of this event
                        write_log(log_id);
                    }
                    break;
                }
            }
        }
        assert(curtime != 0);
        assert(job_id != 0);
        assert(log_id != 0);

        return calc_result();
    }
};

void write_log(ofstream* fp, double* data){
	if(fp -> is_open()){
		(*fp) << data[0] << " " 
                << data[1] << " "
                << data[2] << " "
                << data[3] << " "
                << data[4] << " "
                << data[5] << " "
                << data[6] << "\n";
	}
}

int main(){
    int sim_len = 500;
    double max_lambda = 0.95;
    double lambda_range[sim_len];
    
    for(int i=0; i<sim_len; i++) lambda_range[i] = max_lambda * ((double)(i+1)/(double)(sim_len));

    for(int j=0; j<100; j++){
        printf("[%d] Iter\n", j+1);
        string filePath = "./log/log"+to_string(j)+".txt";
        ofstream writeFile(filePath.data());
        writeFile << "lambda mu traffic_load total_time waiting_by_job njob_avg nq_avg\n";

        for(int i=sim_len-1; i>=0; i--){
            Simulator simulator = Simulator(lambda_range[i], 1); // (lambda, mu)
            write_log(&writeFile, simulator.simulate(10000.0, false));
            if((i % 50 == 0) && (j == 99)) simulator.log_to_file("./log/detail/detailed_"+to_string(lambda_range[i])+"_log.txt");
        }
        writeFile.close();
    }

    {
        string filePath = "./log/1_log.txt";
        ofstream writeFile(filePath.data());
        writeFile << "lambda mu traffic_load total_time waiting_by_job njob_avg nq_avg\n";
        Simulator simulator = Simulator(1, 1);
        write_log(&writeFile, simulator.simulate(10000.0, false));
        simulator.log_to_file("./log/detail/detailed_1_log.txt");
        writeFile.close();
    }

    {
        string filePath = "./log/1_5_log.txt";
        ofstream writeFile(filePath.data());
        writeFile << "lambda mu traffic_load total_time waiting_by_job njob_avg nq_avg\n";
        Simulator simulator = Simulator(1.5, 1);
        write_log(&writeFile, simulator.simulate(10000.0, false));
        simulator.log_to_file("./log/detail/detailed_1_5_log.txt");
        writeFile.close();
    }

    system("python analyze.py");
    system("gnuplot \"pltscript.gnuplot\"");
}