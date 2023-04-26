#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"

uint64
sys_exit(void)
{
  int n;
  argint(0, &n);
  exit(n);
  return 0;  // not reached
}

uint64 sys_info(void)
{
    int n;
    argint(0, &n);
    uint64 result = -1; // Default return value for unsupported cases

    if (n == 0) {
        result = proc_counts();
    }
    else if (n == 1) {
        result = sys_count;
    }
    else if (n == 2) {
        result = k_free_pgs();
    }

    return result;
}


uint64 sys_procinfo(void)
{
    uint64 ptr;
    argaddr(0, &ptr);
    struct proc *p = myproc();

    int temp;
    if ((p->sz) % PGSIZE == 0)
        temp = p->sz / PGSIZE;
    else
        temp = p->sz / PGSIZE + 1;

    if (copyout(p->pagetable, ptr, (char *) &(p->parent->pid), sizeof(p->parent->pid)) < 0 ||
        copyout(p->pagetable, ptr + 4, (char *) &(p->curr_proc), sizeof(p->curr_proc)) < 0 ||
        copyout(p->pagetable, ptr + 8, (char *) &temp, sizeof(p->sz)) < 0) {
        return -1;
    }

    return 0;
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return fork();
}

uint64
sys_wait(void)
{
  uint64 p;
  argaddr(0, &p);
  return wait(p);
}

uint64
sys_sbrk(void)
{
  uint64 addr;
  int n;

  argint(0, &n);
  addr = myproc()->sz;
  if(growproc(n) < 0)
    return -1;
  return addr;
}

uint64
sys_sleep(void)
{
  int n;
  uint ticks0;

  argint(0, &n);
  acquire(&tickslock);
  ticks0 = ticks;
  while(ticks - ticks0 < n){
    if(killed(myproc())){
      release(&tickslock);
      return -1;
    }
    sleep(&ticks, &tickslock);
  }
  release(&tickslock);
  return 0;
}

uint64
sys_kill(void)
{
  int pid;

  argint(0, &pid);
  return kill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}
