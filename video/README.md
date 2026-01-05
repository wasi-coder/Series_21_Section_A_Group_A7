# **🍚 kacchiOS: A Minimal Multitasking OS**

**kacchiOS** is a minimal, educational x86 bare-metal operating system built from scratch. It is designed to demonstrate core OS concepts such as bootloading, serial communication, memory management, and cooperative multitasking through a round-robin scheduler.

## **🚀 Current Features**

### **💻 Multitasking & Scheduling**

* **Cooperative Multitasking:** Implements a round-robin scheduler that switches between tasks using context switching.  
* **Process Management:** Supports process creation, execution, and termination. The kernel manages a `process_table` with states like `READY`, `CURRENT`, and `TERMINATED`.  
* **Context Switching:** Low-level assembly (`switch.S`) handles saving and restoring CPU registers (`EBX`, `ESI`, `EDI`, `EBP`) during task transitions.

### **🐚 Interactive Shell**

* **Serial Console:** A shell process that runs over the COM1 serial port.  
* **Real-time Interaction:** Users can type commands that are echoed back by the OS.  
* **Output Alignment:** Features a `user_is_typing` flag to prevent background tasks from interrupting the user's current line in the shell.

### **🛠️ Kernel Core**

* **Memory Allocator:** A simple "bump allocator" that provides 4KB-aligned memory segments for process stacks.  
* **Bootloader:** Multiboot-compliant assembly code that initializes the stack and clears the BSS section before jumping to C code.  
* **String Library:** Custom implementation of essential functions like `strlen`, `strcmp`, and `strcpy`.

---

## **📁 Project Structure**

* `boot.S`: Entry point and multiboot header.  
* `kernel.c`: Main kernel logic, shell process, and demonstration tasks.  
* `process.c` / `process.h`: Process table and scheduler implementation.  
* `switch.S`: Assembly routine for hardware-level context switching.  
* `memory.c` / `memory.h`: Kernel memory initialization and allocation.  
* `serial.c` / `serial.h`: Driver for COM1 serial communication.  
* `link.ld`: Linker script defining the kernel memory layout (starting at 1MB).

---

## **🛠️ Build and Run**

### **Prerequisites**

You will need `gcc` (multilib for 32-bit), `ld`, and `qemu-system-i386`.

### **Build the Kernel**

Bash

make clean

make

### **Run in QEMU**

To run the OS with serial output in your terminal:

Bash

make run

To run with a VGA window and serial monitoring:

Bash

make run-vga

---

## **📝 Demo Execution**

When you run kacchiOS, the following sequence occurs:

1. The kernel initializes serial I/O and memory.  
2. Three processes are created: `shell_task`, `task_a` (background), and `task_finite`.  
3. The scheduler starts, and you will see:  
   * **Background Task:** Periodically prints status messages.  
   * **Finite Task:** Prints "I am alive\!" three times and then terminates cleanly.  
   * **Interactive Shell:** Allows you to type input, which silences background logs while you are active.

---

## **📄 License**

This project is licensed under the **MIT License**.

