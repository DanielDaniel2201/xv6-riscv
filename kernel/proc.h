// Saved registers for kernel context switches.
struct context {
  uint64 ra;
  uint64 sp;

  // callee-saved
  uint64 s0;
  uint64 s1;
  uint64 s2;
  uint64 s3;
  uint64 s4;
  uint64 s5;
  uint64 s6;
  uint64 s7;
  uint64 s8;
  uint64 s9;
  uint64 s10;
  uint64 s11;
};

// Per-CPU state.
struct cpu {
  struct proc *proc;          // The process running on this cpu, or null.
  struct context context;     // swtch() here to enter scheduler().
  int noff;                   // Depth of push_off() nesting.
  int intena;                 // Were interrupts enabled before push_off()?
};

extern struct cpu cpus[NCPU];

// per-process data for the trap handling code in trampoline.S.
// sits in a page by itself just under the trampoline page in the
// user page table. not specially mapped in the kernel page table.
// uservec in trampoline.S saves user registers in the trapframe,
// then initializes registers from the trapframe's
// kernel_sp, kernel_hartid, kernel_satp, and jumps to kernel_trap.
// usertrapret() and userret in trampoline.S set up
// the trapframe's kernel_*, restore user registers from the
// trapframe, switch to the user page table, and enter user space.
// the trapframe includes callee-saved user registers like s0-s11 because the
// return-to-user path via usertrapret() doesn't return through
// the entire kernel call stack.
struct trapframe {
  /*   0 */ uint64 kernel_satp;   // kernel page table
  /*   8 */ uint64 kernel_sp;     // top of process's kernel stack
  /*  16 */ uint64 kernel_trap;   // usertrap()
  /*  24 */ uint64 epc;           // saved user program counter
  /*  32 */ uint64 kernel_hartid; // saved kernel tp
  /*  40 */ uint64 ra;
  /*  48 */ uint64 sp;
  /*  56 */ uint64 gp;
  /*  64 */ uint64 tp;
  /*  72 */ uint64 t0;
  /*  80 */ uint64 t1;
  /*  88 */ uint64 t2;
  /*  96 */ uint64 s0;
  /* 104 */ uint64 s1;
  /* 112 */ uint64 a0;
  /* 120 */ uint64 a1;
  /* 128 */ uint64 a2;
  /* 136 */ uint64 a3;
  /* 144 */ uint64 a4;
  /* 152 */ uint64 a5;
  /* 160 */ uint64 a6;
  /* 168 */ uint64 a7;
  /* 176 */ uint64 s2;
  /* 184 */ uint64 s3;
  /* 192 */ uint64 s4;
  /* 200 */ uint64 s5;
  /* 208 */ uint64 s6;
  /* 216 */ uint64 s7;
  /* 224 */ uint64 s8;
  /* 232 */ uint64 s9;
  /* 240 */ uint64 s10;
  /* 248 */ uint64 s11;
  /* 256 */ uint64 t3;
  /* 264 */ uint64 t4;
  /* 272 */ uint64 t5;
  /* 280 */ uint64 t6;
};

enum procstate { UNUSED, USED, SLEEPING, RUNNABLE, RUNNING, ZOMBIE };

// Per-process state
struct proc {
  struct spinlock lock; //                                                     -->24 bytes

  // p->lock must be held when using these:
  enum procstate state;        // Process state                                --> 4 bytes
  void *chan;                  // If non-zero, sleeping on chan                --> 8 bytes
  int killed;                  // If non-zero, have been killed                --> 4 bytes
  int xstate;                  // Exit status to be returned to parent's wait  --> 4 bytes
  int pid;                     // Process ID                                   --> 4 bytes

  // wait_lock must be held when using this:
  struct proc *parent;         // Parent process                               --> 8 btyes

  // these are private to the process, so p->lock need not be held.
  uint64 kstack;               // Virtual address of kernel stack              --> 8 bytes
  uint64 sz;                   // Size of process memory (bytes)               --> 8 bytes
  pagetable_t pagetable;       // User page table                              --> 8 bytes
  struct trapframe *trapframe; // data page for trampoline.S                   --> 8 bytes
  struct context context;      // swtch() here to run process                  --> 112 bytes
  struct file *ofile[NOFILE];  // Open files                                   --> 128 bytes (16 * 8)
  struct inode *cwd;           // Current directory                            --> 8 bytes
  char name[16];               // Process name (debugging)                     --> 16 bytes
};//                                                                           ==> ( 348 ) 360 bytes overall for a proc struct


// type = struct proc {
// /*      0      |      24 */    struct spinlock {
// /*      0      |       4 */        uint locked;
// /* XXX  4-byte hole      */
// /*      8      |       8 */        char *name;
// /*     16      |       8 */        struct cpu *cpu;

//                                    /* total size (bytes):   24 */
//                                } lock;
// /*     24      |       4 */    enum procstate state;
// /* XXX  4-byte hole      */
// /*     32      |       8 */    void *chan;
// /*     40      |       4 */    int killed;
// /*     44      |       4 */    int xstate;
// /*     48      |       4 */    int pid;
// /* XXX  4-byte hole      */
// /*     56      |       8 */    struct proc *parent;
// /*     64      |       8 */    uint64 kstack;
// /*     72      |       8 */    uint64 sz;
// /*     80      |       8 */    pagetable_t pagetable;
// /*     88      |       8 */    struct trapframe *trapframe;
// /*     96      |     112 */    struct context {
// /*     96      |       8 */        uint64 ra;
// /*    104      |       8 */        uint64 sp;
// /*    112      |       8 */        uint64 s0;
// /*    120      |       8 */        uint64 s1;
// /*    128      |       8 */        uint64 s2;
// /*    136      |       8 */        uint64 s3;
// /*    144      |       8 */        uint64 s4;
// /*    152      |       8 */        uint64 s5;
// /*    160      |       8 */        uint64 s6;
// /*    168      |       8 */        uint64 s7;
// /*    176      |       8 */        uint64 s8;
// /*    184      |       8 */        uint64 s9;
// /*    192      |       8 */        uint64 s10;
// --Type <RET> for more, q to quit, c to continue without paging--c
// /*    200      |       8 */        uint64 s11;

//                                    /* total size (bytes):  112 */
//                                } context;
// /*    208      |     128 */    struct file *ofile[16];
// /*    336      |       8 */    struct inode *cwd;
// /*    344      |      16 */    char name[16];

//                                /* total size (bytes):  360 */
//                              } [64]