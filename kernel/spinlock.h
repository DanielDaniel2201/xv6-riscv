// Mutual exclusion lock.
struct spinlock {
  uint locked;       // Is the lock held?

  // For debugging:
  char *name;        // Name of lock.
  struct cpu *cpu;   // The cpu holding the lock.
};
// 24 bytes overall for a spinlock struct
// type = struct spinlock {
// /*      0      |      24 */    struct spinlock {
// /*      0      |       4 */        uint locked;
// /* XXX  4-byte hole      */
// /*      8      |       8 */        char *name;
// /*     16      |       8 */        struct cpu *cpu;

//                                    /* total size (bytes):   24 */
//                                } lock;