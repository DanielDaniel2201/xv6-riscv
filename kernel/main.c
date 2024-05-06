#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "riscv.h"
#include "defs.h"

volatile static int started = 0;

// start() jumps here in supervisor mode on all CPUs.
void
main()
{
  if(cpuid() == 0){
    consoleinit();
    printfinit();
    printf("\n");
    printf("xv6 kernel is booting\n");
    printf("\n");
    // kinit(): add all pages between "end" and PHYSTOP to kmem.freelist
    // all the pages in that range can be used through calling kalloc()
    kinit();         // physical page allocator
    // kvminit(): prepare the entire kernel virtual address space (for later use) by setting up kernel_pagetable
    // allocate a page to store kernel page table by calling kalloc() for the first time
    // map *UART0*, *VIRTIO0*, *PLIC*, *.text*, *data and RAM*, *trampoline*, and *32 kernel stacks and 32 guard pages for each possible process*
    kvminit();       // create kernel page table
    // kvminithar(): turn on paging by write kernel_pagetable address to satp register
    kvminithart();   // turn on paging
    // for each process initialize its lock and map proc->kstack to the kernel stack initialized earlier in kvminit()
    procinit();      // process table
    // trapinit(): initialize its lock
    trapinit();      // trap vectors
    // trapinithart(): write the address of kernelvec into stvec register
    trapinithart();  // install kernel trap vector
    plicinit();      // set up interrupt controller
    plicinithart();  // ask PLIC for device interrupts
    binit();         // buffer cache
    iinit();         // inode table
    fileinit();      // file table
    virtio_disk_init(); // emulated hard disk
    userinit();      // first user process
    __sync_synchronize();
    started = 1;
  } else {
    while(started == 0)
      ;
    __sync_synchronize();
    printf("hart %d starting\n", cpuid());
    kvminithart();    // turn on paging
    trapinithart();   // install kernel trap vector
    plicinithart();   // ask PLIC for device interrupts
  }

  scheduler();        
}
