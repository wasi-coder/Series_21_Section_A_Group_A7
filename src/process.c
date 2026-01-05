/* process.c - With Termination Support */
#include "process.h"
#include "memory.h"
#include "types.h"
#include "serial.h"

#define MAX_PROC 10
static pcb_t process_table[MAX_PROC];
static int current_pid = -1;//starts at no process yet so -1
static int proc_count = 0; //number of processes created so far 

extern void context_switch(uint32_t* old_esp, uint32_t new_esp);

void process_init(void) {
    for(int i = 0; i < MAX_PROC; i++) {
        process_table[i].state = TERMINATED;
    }
}

/* 1. NEW: The Exit Function */
void process_exit(void) {
    process_table[current_pid].state = TERMINATED;
    serial_puts("Process Terminated.\n");
    schedule(); // Immediately switch to someone else
    
    // We should never return here.
    while(1); 
}

int process_create(void (*entry)(void)) {
    if (proc_count >= MAX_PROC) return -1;
    
    pcb_t* p = &process_table[proc_count++];
    uint32_t* stack = (uint32_t*)kalloc(4096); //4kb er stack 
    uint32_t* esp = (uint32_t*)((uint32_t)stack + 4096); //stack er top a point kortase 
    //as the stackj grows downwards, we start at the top

    
    /* 2. UPDATE: Setup stack to auto-exit */
    // If 'entry' function returns, it will "return" to process_exit
    *(--esp) = (uint32_t)process_exit; // Return address
    
    *(--esp) = (uint32_t)entry; // The actual function to run
    
    // Dummy registers for context_switch
    *(--esp) = 0; // EBP
    *(--esp) = 0; // EDI
    *(--esp) = 0; // ESI
    *(--esp) = 0; // EBX

    p->esp = (uint32_t)esp;
    p->state = READY;
    return 0;
}

void schedule(void) {
    int next_pid = current_pid;
    int attempts = 0;

    /* 3. UPDATE: Find next READY process (Skip TERMINATED ones) */
    do {
        next_pid = (next_pid + 1) % proc_count;
        attempts++;
        if (attempts > proc_count) {
            // No ready processes left? Just hang or return to shell if it's 0
            // For safety, we just idle if everyone is dead
            if (process_table[current_pid].state == TERMINATED) {
                 // In a real OS, we'd halt. Here we just spin.
                 // Ideally, the idle task or shell never dies.
                 return; 
            }
            next_pid = current_pid; 
            break;
        }
    } while (process_table[next_pid].state != READY);

    int old_pid = current_pid;
    current_pid = next_pid;
    
    pcb_t* new_proc = &process_table[current_pid];
    pcb_t* old_proc = (old_pid == -1) ? NULL : &process_table[old_pid];

    new_proc->state = CURRENT;
    
    // Only mark the old one as READY if it wasn't dead
    if (old_proc && old_proc->state != TERMINATED) {
        old_proc->state = READY;
    }

    context_switch(old_proc ? &old_proc->esp : NULL, new_proc->esp);
}

void yield(void) {
    schedule();
}