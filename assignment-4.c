#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdbool.h>
#include <pthread.h>

typedef struct {
    int pid;
    int arrival_time;
    int original_burst_time;
    int remaining_burst_time;
    int wait_time;
    bool active_process;
    int turnaround_time;
    bool is_completed;
    int time_left; 
} Process;

Process processes[100];
int num_processes = 0;

int main(int argc, char *argv[]) {
    //Checks if the length of what the user type is more than 3 or less than 1, if so then error
    if (argc > 4 || argc < 2) {
        printf("Please type in 2 or 3 parameters only");
        return 1;
    }
    char algorithm;
    int time_quantum;
    char *file_name;
    float total_wait = 0.0;
    float total_turnaround = 0.0;
    char line[1000];

    if (strcmp(argv[1], "-f") == 0) {
        //Sets the scheduling flag to First Come First Serve
        printf("First Come First Served\n");
        algorithm = 'f';
        file_name = argv[2];
        
    }
    else if (strcmp(argv[1], "-s") == 0) {
        //Sets the scheduling flag to Shortest Job First
        printf("Shortest Job First \n");
        algorithm = 's';
        file_name = argv[2];
    }
    else if (strcmp(argv[1], "-r") == 0) {
        //Checks if the user types a time quantum with the scheduling type
        if (argc != 4) {
            printf("Round Robin requires a time quantum.\n");
            return 1;
        }
        //store the time quantum and set scheduling flag to Round Robin
        time_quantum = atoi(argv[2]);
        printf("Round Robin with Quantum %d \n", time_quantum);
        algorithm = 'r';
        
        file_name = argv[3];
    } 
    else {
        //In the case of invalid scheduling flag
        printf("Please type a valid scheduling flag\n");
        return 1;
    }
    //Reading the file
    FILE *file = fopen(file_name, "r");
    int burst;
    //Storing the values inside our processor
    while (fgets(line, sizeof(line), file)) {
        if (sscanf(line, "P%*d,%d", &burst) == 1) {
            processes[num_processes].pid = num_processes;
            processes[num_processes].arrival_time = num_processes; 
            processes[num_processes].original_burst_time = burst;
            processes[num_processes].remaining_burst_time = burst;
            processes[num_processes].wait_time = 0;
            processes[num_processes].turnaround_time = 0;
            processes[num_processes].time_left = false;
            processes[num_processes].is_completed = false;
            num_processes++;
        }
    }
    //Closing file
    fclose(file);
    int current_tick = 0; 
    int is_complete = 0;
    int active_pid = -1;
    int current_quantum_time = 0;
    while (is_complete < num_processes) {
        //select the process with algorithm f
        if (algorithm == 'f') {
            //uses a non-preemptive approach to pick new if none are active or everything is currently finished
            if (active_pid == -1 || processes[active_pid].is_completed) {
                for (int num = 0; num < num_processes; num++) {
                    if (processes[num].arrival_time <= current_tick && !processes[num].is_completed) {
                        active_pid = num;
                        break; 
                    }
                }
            }
        }
        else if (algorithm == 's') {
            //selects process with algorithm s
            int next_pid = 0;
            int minimum_burst = 100000;
            for (int num =0; num < num_processes; num++) {
                //uses a preemptive approach
                if (processes[num].arrival_time <= current_tick && !processes[num].is_completed) {
                    if (processes[num].remaining_burst_time < minimum_burst) {
                        minimum_burst = processes[num].remaining_burst_time;
                        next_pid = num;
                    } 
                    // Tie-breaker: If bursts are equal, prioritize the currently running process
                    else if (processes[num].remaining_burst_time == minimum_burst) {
                        if (num == active_pid) {
                            next_pid = num; 
                        }
                    }
                }
            }
            active_pid = next_pid;
        }
        else if (algorithm == 'r') {
            //uses a round robin approach 
            bool algor_switch = false; 
            if (active_pid == -1 || processes[active_pid].is_completed || current_quantum_time == time_quantum) {
                algor_switch = true;
            }
            if (algor_switch) {
                int start_check = (active_pid == -1) ? 0 : (active_pid + 1) % num_processes;
                int next_pid = -1;
                for (int num = 0; num < num_processes; num++) {
                    int check_pid = (start_check + num) % num_processes;
                    if (processes[check_pid].arrival_time <= current_tick && !processes[check_pid].is_completed) {
                        next_pid = check_pid;
                        break;
                    }
                }
                if (next_pid != -1) {
                    active_pid = next_pid;
                    current_quantum_time = 0; 
                } else if (active_pid != -1 && !processes[active_pid].is_completed) {
                    current_quantum_time = 0;
                }
            }
        }

        if (active_pid != -1) {
            
            printf("T%-4d: P%-4d - Burst left %4d, Wait time %4d, Turnaround time %4d\n", 
                   current_tick, active_pid, 
                   processes[active_pid].remaining_burst_time, 
                   processes[active_pid].wait_time, 
                   processes[active_pid].turnaround_time);
            for (int num = 0; num < num_processes; num++) {
                if (processes[num].arrival_time <= current_tick && !processes[num].is_completed) {
                    processes[num].turnaround_time++;
                    if (num != active_pid) {
                        processes[num].wait_time++;
                    }
                }
            }
            processes[active_pid].remaining_burst_time--;
            if (processes[active_pid].remaining_burst_time == 0) {
                processes[active_pid].is_completed = true;
                is_complete++;
            }
            
            current_quantum_time++;
        }
        current_tick++;
    }
    for (int num = 0; num < num_processes; num++) {
        printf("P%d\n", num);
        printf("Waiting time: %d\n", processes[num].wait_time);
        printf("Turnaround time:%d\n", processes[num].turnaround_time);
        total_wait += processes[num].wait_time;
        total_turnaround += processes[num].turnaround_time;
    }
    printf("Total average waiting time:     %.4f\n", total_wait / num_processes);
    printf("Total average turnaround time:  %.4f\n", total_turnaround / num_processes);
    return 0;

}
