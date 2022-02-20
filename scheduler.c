#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <fcntl.h>
#include <string.h>

typedef struct job {
    int id;
    int length;
    struct job *next;
    struct job *prev;
} job_t;

job_t *head;

void remove_node(job_t *node) {
    if (node->prev != (job_t *) NULL) {
        if (node->next != (job_t *) NULL) {
            node->prev->next = node->next;
            node->next->prev = node->prev;
        }
        else {
            node->prev->next = (job_t *) NULL;
        }
    }
    else {
        head = node->next;
        if (node->next != NULL) {
            node->next->prev = (job_t *) NULL;
        }
    }
}

void fifo(){
    job_t *node = head;
    while (node != NULL) {
        printf("Job %d ran for: %d\n", node->id, node->length);
        node = node->next;
    }
}

void sjf() {
    job_t *node = head;
    job_t *scheduled;
    int shortest = INT_MAX;
    while (head != (job_t *) NULL) {
        while (node != (job_t *) NULL) {
            if (node->length < shortest) {
                shortest = node->length;
                scheduled = node;
            }
            node = node->next;
        }
        printf("Job %d ran for: %d\n", scheduled->id, scheduled->length);
        
        remove_node(scheduled);
        
        node = head;
        shortest = INT_MAX;
    }
}

void rr(int slice) {
    job_t *node = head;
    while (head != (job_t *) NULL) {
        while (node != (job_t *) NULL) {
            if (node->length>slice) {
                printf("Job %d ran for: %d\n", node->id, slice);
                node->length-=slice;
            }
            else {
                printf("Job %d ran for: %d\n", node->id, node->length);
                remove_node(node);
            }
            node = node->next;
        }
        node = head;
    }
}

int main(int argc, char **argv){
    char* policy = *(argv+1);
    char* workload = *(argv+2);
    int timeslice = atoi(*(argv+3));

    char* buffer;
    size_t buf_size = 5;
    buffer = (char *)malloc(buf_size);

    FILE * file;
    file = fopen(workload,"r");
    job_t *prev_job = NULL;
    int counter = 0;
    while(getline(&buffer, &buf_size, file) != -1) {
        int len = atoi(buffer);
        job_t *curr_job = (job_t *)malloc(sizeof(job_t));
        curr_job->id = counter;
        curr_job->length = len;
        if (prev_job != (job_t *) NULL) {
            prev_job->next = curr_job;
            curr_job->prev = prev_job;
        }
        else {
            head = curr_job;
        }
        prev_job = curr_job;
        counter++;
    }
    fclose(file);

    printf("Execution trace with %s:\n", policy);

    if (strcmp(policy, "FIFO") == 0) {
        fifo();
    }
    else if (strcmp(policy, "SJF") == 0) {
        sjf();
    }
    else if (strcmp(policy, "RR") == 0) {
        rr(timeslice);
    }
    else {
        printf("Invalid scheduling policy: %s\n", policy);
    }
    
    printf("End of execution with %s.\n", policy);

    return EXIT_SUCCESS;
}