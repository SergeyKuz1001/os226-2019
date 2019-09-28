#include "sched.h"
#include <stdio.h>
#include <string.h>

struct sched_app {
    void *func;
    struct sched_app *next_app;
    unsigned time_continue;
    unsigned priority;
    unsigned deadline;
    void *aspace;
};

short policy = 0;
unsigned time = 1;

struct sched_app apps[16];
unsigned apps_n = 0;
struct sched_app *head_queue = 0;
struct sched_app *running_app = 0;

void sched_new(void (*entrypoint)(void *aspace), void *aspace, int priority, int deadline) {
    apps[apps_n].func = entrypoint;
    apps[apps_n].next_app = 0;
    apps[apps_n].time_continue = 1;
    apps[apps_n].priority = priority;
    apps[apps_n].deadline = deadline;
    apps[apps_n].aspace = aspace;
    apps_n++;
}

void sched_cont(void (*entrypoint)(void *aspace), void *aspace, int timeout) {
    running_app->func = entrypoint;
    running_app->aspace = aspace;
    running_app->time_continue = time + timeout;
}

void sched_time_elapsed(unsigned amount) {
    time += amount;
}

void sched_set_policy(const char *name) {
    if (!strcmp(name, "fifo")) {
        policy = 1;
        return;
    }
    if (!strcmp(name, "priority")) {
        policy = 2;
        return;
    }
    if (!strcmp(name, "deadline")) {
        policy = 3;
        return;
    }
    printf("Error: %s - unknown policy\n", name);
}

void sched_run(void) {
    if (!policy) {
        printf("Error: policy was not give.\n");
        return;
    }

    for (int i = 0; i < apps_n; i++) {
        struct sched_app *a = head_queue;
        struct sched_app *pred_a = 0;
        while (a) {
            if (policy == 2 && a->priority > apps[i].priority)
                break;
            if (policy == 3 && (a->deadline > apps[i].deadline || a->deadline == apps[i].deadline && a->priority > apps[i].priority))
                break;
            pred_a = a;
            a = a->next_app;
        }
        if (pred_a)
            pred_a->next_app = apps + i;
        else
            head_queue = apps + i;
        apps[i].next_app = a;
    }

    while (head_queue) {
        int min_time_continue = head_queue->time_continue;
        struct sched_app *pred_running_app = 0;
        running_app = head_queue;
        while (running_app) {
            if (running_app->time_continue <= time)
                break;
            if (policy == 2 && running_app->next_app && running_app->next_app->priority > running_app->priority)
                break;
            if (policy == 3 && running_app->next_app && running_app->next_app->deadline > running_app->deadline)
                break;
            if (min_time_continue > running_app->time_continue)
                min_time_continue = running_app->time_continue;
            pred_running_app = running_app;
            running_app = running_app->next_app;
        }
        if (!running_app) {
            time = min_time_continue;
            continue;
        }
        running_app->time_continue = 0;
        void (*app_entrypoint)(void*) = running_app->func;
        (*app_entrypoint)(running_app->aspace);
        if (!running_app->time_continue) {
            if (pred_running_app)
                pred_running_app->next_app = running_app->next_app;
            else
                head_queue = running_app->next_app;
        }
    }
}
