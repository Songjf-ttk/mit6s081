// Physical memory allocator, for user processes,
// kernel stacks, page-table pages,
// and pipe buffers. Allocates whole 4096-byte pages.

#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "riscv.h"
#include "defs.h"
#include "proc.h"

void freerange(void *pa_start, void *pa_end);

extern char end[]; // first address after kernel.
                   // defined by kernel.ld.

struct run {
  struct run *next;
};

struct {
  struct spinlock lock;
  struct run *freelist;
} kmem;

struct {
  struct spinlock lock;
  uint64 counts[PA2NUM(PHYSTOP)+1];
}pgcnt_refer;


int
P(uint64 pa){
  int t,id;
  id = PA2NUM(pa);
  if(id<0||id>PA2NUM(PHYSTOP)) panic("V: id error");
  acquire(&pgcnt_refer.lock);
  t = --pgcnt_refer.counts[id];
  release(&pgcnt_refer.lock);
  return t;
}

int
V(uint64 pa)
{
  int t,id;
  id = PA2NUM(pa);
  if(id<0||id>PA2NUM(PHYSTOP)) panic("V: id error");
  acquire(&pgcnt_refer.lock);
  t = ++pgcnt_refer.counts[id];
  release(&pgcnt_refer.lock);
  return t;
}
void
kinit()
{
  initlock(&kmem.lock, "kmem");
  initlock(&pgcnt_refer.lock, "cow");
  freerange(end, (void*)PHYSTOP);
}

void
freerange(void *pa_start, void *pa_end)
{
  char *p;
  p = (char*)PGROUNDUP((uint64)pa_start);
  for(int i = 0;i<sizeof pgcnt_refer.counts / sizeof (uint64);i++)
    pgcnt_refer.counts[i] = 1;
  for(; p + PGSIZE <= (char*)pa_end; p += PGSIZE)
    kfree(p);
}

// Free the page of physical memory pointed at by v,
// which normally should have been returned by a
// call to kalloc().  (The exception is when
// initializing the allocator; see kinit above.)
void
kfree(void *pa)
{
  struct run *r;
  uint64 count;

  if(((uint64)pa % PGSIZE) != 0 || (char*)pa < end || (uint64)pa >= PHYSTOP)
    panic("kfree");

  // Fill with junk to catch dangling refs.
  memset(pa, 1, PGSIZE);

  r = (struct run*)pa;

  acquire(&kmem.lock);
  count = P((uint64)pa);
  if(count < 0)
    panic("kfree: the num of used shouldn't below zero");
  if(count == 0){
    r->next = kmem.freelist;
    kmem.freelist = r;
  }
  release(&kmem.lock);
}

// Allocate one 4096-byte page of physical memory.
// Returns a pointer that the kernel can use.
// Returns 0 if the memory cannot be allocated.
void *
kalloc(void)
{
  struct run *r;

  acquire(&kmem.lock);
  r = kmem.freelist;
  if(r)
    kmem.freelist = r->next;
  V((uint64)r);
  release(&kmem.lock);

  if(r)
    memset((char*)r, 5, PGSIZE); // fill with junk
  return (void*)r;
}
