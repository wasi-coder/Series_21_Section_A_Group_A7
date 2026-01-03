#ifndef PROCESS_H
#define PROCESS_H
#include "types.h"

typedef enum { READY, CURRENT, TERMINATED } state_t;

typedef struct {
    uint32_t esp;       // Saved stack pointer
    state_t state;      // Process state
    uint32_t pid;       // Process ID
} pcb_t;

void process_init(void);
int process_create(void (*entry_point)(void));
void schedule(void); // The Scheduler
void yield(void);    // Voluntarily give up CPU

#endif