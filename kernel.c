/* kernel.c - Unified Kernel with Shell and Multitasking */
#include "types.h"
#include "serial.h"
#include "string.h"
#include "memory.h"   /* Required for memory_init */
#include "process.h"  /* Required for process_init, process_create, and yield */

#define MAX_INPUT 128

/* A background task to demonstrate multitasking */
void task_a(void) {
    while(1) {
        serial_puts("[Background Task] Running...\n");
        for(volatile int i = 0; i < 10000000; i++); // Busy wait delay
        yield(); // Give control back to the scheduler
    }
}

/* The original shell functionality converted into a process */
void shell_task(void) {
    char input[MAX_INPUT];
    int pos = 0;

    serial_puts("Shell Process Started.\n");

    while (1) {
        serial_puts("kacchiOS> ");
        pos = 0;
        
        while (1) {
            char c = serial_getc();
            
            if (c == '\r' || c == '\n') {
                input[pos] = '\0';
                serial_puts("\n");
                break;
            }
            else if ((c == '\b' || c == 0x7F) && pos > 0) {
                pos--;
                serial_puts("\b \b");
            }
            else if (c >= 32 && c < 127 && pos < MAX_INPUT - 1) {
                input[pos++] = c;
                serial_putc(c);
            }
            
            // To prevent the shell from hogging the CPU, 
            // we yield frequently while waiting for input.
            yield(); 
        }
        
        if (pos > 0) {
            serial_puts("You typed: ");
            serial_puts(input);
            serial_puts("\n");
        }
    }
}

/* The single Entry Point for the Kernel */
void kmain(void) {
    /* 1. Initialize hardware */
    serial_init();
    
    serial_puts("\n========================================\n");
    serial_puts("    kacchiOS - Multitasking Enabled\n");
    serial_puts("========================================\n");

    /* 2. Initialize OS Managers */
    memory_init();
    process_init();

    /* 3. Create Processes */
    process_create(shell_task); // Process 0: The Shell
    process_create(task_a);     // Process 1: Background Task

    serial_puts("Starting Scheduler...\n\n");

    /* 4. Start the first process */
    schedule();

    /* Should never reach here */
    for (;;) {
        __asm__ volatile ("hlt");
    }
}