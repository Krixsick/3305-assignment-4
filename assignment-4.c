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
    int burst_time;
    int wait_time;
    bool active_process;
    int turnaround_time;
    int time_left; 
} Process;

Process processes[100];
int num_processes = 0;

int main(int argc, char *argv[]) {
    if (argc > 3 || argc < 1) {
        printf("Please type in 2 or 3 parameters only");
        return 1;
    }
}
