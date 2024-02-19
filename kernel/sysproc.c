#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"
#include "sysinfo.h"

uint64 collect_freem();
uint64 collect_npro();

uint64
sys_exit(void)
{
  int n;
  argint(0, &n);
  exit(n);
  return 0;  // not reached
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
  if(n < 0)
    n = 0;
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

// sys_trace
uint64
sys_trace(void)
{
  int mask;
  argint(0, &mask);

// 添加一个变量默认为零，如果有输入则保存在一个变量中，syscall在每次调用的过程中对变量进行检查，如果调用和变量数值相同，则打出相关调用
  
  struct proc *p = myproc();
  p->trace = mask;
  return 0;
}

uint64
sys_sysinfo(void)
{
  struct sysinfo s;
  struct proc *p = myproc();

  uint64 userSpace; // user pointer to array of two integers
  argaddr(0, &userSpace);

  s.freemem = collect_freem();
  s.nproc = collect_npro();

  copyout(p->pagetable, userSpace, (char*)&s, sizeof(s));

  return 0;
}
