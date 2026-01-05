/* kernel.c - Unified Kernel with Output Alignment Fix */
#include "types.h"
#include "serial.h"
#include "string.h"
#include "memory.h"
#include "process.h"

#define MAX_INPUT 128

/* * Global flag to prevent background tasks from messing up 
 * the screen while the user is typing.
 */
volatile int user_is_typing = 0;

/* A background task to demonstrate multitasking */
void task_a(void) {
    while(1) {
        /* Only print if the user is NOT typing */
        if (!user_is_typing) {
            /* * \r -> Go to start of line (overwrite the idle prompt)
             * Print the message
             * \n -> New line
             * Reprint the prompt so the user knows they can type
             */
            serial_puts("\r[Background Task] Running...\n");
            serial_puts("kacchiOS> ");
        }

        /* Delay loop */
        for(volatile int i = 0; i < 2000000000; i++); 
        
        yield(); // Give control back to the scheduler
    }
}

/* The shell process */
void shell_task(void) {
    char input[MAX_INPUT];
    int pos = 0;

    serial_puts("Shell Process Started.\n");

    while (1) {
        serial_puts("kacchiOS> ");
        pos = 0;
        user_is_typing = 0; // User is at a fresh prompt
        
        while (1) {
            /* Wait for data, yield if none available */
            while (!serial_received()) {
                yield();
            }

            char c = serial_getc();
            
            /* Handle Enter key */
            if (c == '\r' || c == '\n') {
                input[pos] = '\0';
                serial_puts("\n");
                user_is_typing = 0; // Command finished, safe to print again
                break;
            }
            /* Handle Backspace */
            else if ((c == '\b' || c == 0x7F) && pos > 0) {
                pos--;
                serial_puts("\b \b");
                /* If they deleted everything, they aren't "typing" anymore */
                if (pos == 0) user_is_typing = 0; 
            }
            /* Handle normal characters */
            else if (c >= 32 && c < 127 && pos < MAX_INPUT - 1) {
                input[pos++] = c;
                serial_putc(c);
                user_is_typing = 1; // User is active! Silence the logs.
            }
        }
        
        /* Echo command (only if they actually typed something) */
        if (pos > 0) {
            serial_puts("You typed: ");
            serial_puts(input);
            serial_puts("\n");
        }
    }
}

void task_finite(void) {
    for (int i = 0; i < 3; i++) {
        serial_puts("[Finite Task] I am alive!\n");
        for(volatile int j=0; j<100000000; j++); // Delay
        yield();
    }
    // Function ends here -> Should auto-call process_exit()
}

/* The single Entry Point for the Kernel */
void kmain(void) {
    serial_init();
    
    serial_puts("\n========================================\n");
    serial_puts("    kacchiOS - Multitasking Enabled\n");
    serial_puts("========================================\n");

    memory_init();// Initialize memory allocator
    process_init();// Initialize process table

    process_create(shell_task);
    process_create(task_a);
    process_create(task_finite);

    serial_puts("Starting Scheduler...\n\n");

    schedule();// Start the first process

    for (;;) {
        __asm__ volatile ("hlt");
    }
}