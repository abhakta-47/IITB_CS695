# CS695 : Assignment 2

This project involves working with **virtualization concepts** and building/modifying a **simple KVM-based hypervisor setup**. You will analyze provided guest and hypervisor code, modify it for specific tasks, and implement a multi-VM communication scenario. The work is divided into **Part 1 (analysis and modification)** and **Part 2 (implementation)**.

---

### **Part 1 – Understanding and Extending Simple KVM**

1. **Part 1a – Code Analysis**

   * Study the given `simple-kvm.c` setup.
   * Explain how memory is allocated, guest code is loaded, and execution is handled.
   * Provide a **flowchart** showing VM initialization and run loop.
   * Submit this as a **PDF** (`part1a.pdf`).

2. **Part 1b – KVM Modifications**

   * Modify the simple KVM setup to:

     * Boot a guest in **protected mode**.
     * Load and execute a provided `guest.c` program.
     * Understand and adjust `guest.ld`, `payload.ld`, and assembly stubs.

---

### **Part 2 – Multi-VM Producer–Consumer via Hypervisor**

* Implement a **non-deterministic producer–consumer** model with **two guest VMs** and a **hypervisor**.
* The **Producer VM**:

  * Produces a random number of items (up to a buffer size of 6).
  * Sends buffer state to the hypervisor.
* The **Hypervisor**:

  * Copies producer’s buffer to consumer VM’s buffer.
  * Passes updated state between VMs.
* The **Consumer VM**:

  * Consumes a random number of items.
  * Sends the updated buffer back to the hypervisor.
* Implement proper **pointer management** (`prod_p`, `cons_p`) to track buffer status.

---

### **Submission Format**

```
<rollnumber>_assignment2/
├── .git/                # Keep starter git repo intact
├── part1a.pdf           # Analysis document
├── part1/               # Modified KVM code
│   ├── guest.c
│   ├── guest.ld
│   ├── guest16.s
│   ├── payload.ld
│   ├── simple-kvm.c
│   ├── Makefile
│   └── README.md
├── part2/               # Producer–consumer implementation
│   ├── emu.c / emuX.c
│   ├── guest*.c
│   ├── guest.ld
│   ├── payload.ld
│   ├── Makefile
│   └── README.md
├── Makefile
└── README.md            # This file
```
