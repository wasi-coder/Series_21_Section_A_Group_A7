/* process.c - Process Manager Implementation */
#include "process.h"
#include "memory.h"
#include "types.h"

#define MAX_PROC 10
static pcb_t process_table[MAX_PROC];
static int current_pid = -1;
static int proc_count = 0;

/* External assembly function defined in switch.S */
extern void context_switch(uint32_t* old_esp, uint32_t new_esp);

void process_init(void) {
    for(int i = 0; i < MAX_PROC; i++) {
        process_table[i].state = TERMINATED;
    }
}

int process_create(void (*entry)(void)) {
    if (proc_count >= MAX_PROC) return -1;
    
    pcb_t* p = &process_table[proc_count++];
    
    /* Allocate 4KB for the process stack */
    uint32_t* stack = (uint32_t*)kalloc(4096);
    
    /* Point to the top of the stack (stacks grow downwards) */
    uint32_t* esp = (uint32_t*)((uint32_t)stack + 4096);

    /* Setup initial stack frame for context_switch */
    *(--esp) = (uint32_t)entry; // Return address (function entry point)
    *(--esp) = 0; // EBP
    *(--esp) = 0; // EDI
    *(--esp) = 0; // ESI
    *(--esp) = 0; // EBX

    p->esp = (uint32_t)esp;
    p->state = READY;
    return 0; // Success
}

void schedule(void) {
    int next_pid = (current_pid + 1) % proc_count;
    
    /* Find the next READY process (Round Robin) */
    // Note: In a real OS, we'd loop here. For now, we assume simple toggling works.
    
    pcb_t* old_proc = (current_pid == -1) ? NULL : &process_table[current_pid];
    current_pid = next_pid;
    pcb_t* new_proc = &process_table[current_pid];

    /* Perform the context switch */
    context_switch(old_proc ? &old_proc->esp : NULL, new_proc->esp);
}

/* The missing function! */
void yield(void) {
    schedule();
}