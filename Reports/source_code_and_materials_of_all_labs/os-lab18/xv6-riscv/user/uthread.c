#include "kernel/types.h"
#include "user/user.h"

#define FREE     0x0
#define RUNNING  0x1
#define RUNNABLE 0x2

#define STACK_SIZE 8192
#define MAX_THREAD 4

/* Layout matches the offsets used by uthread_switch.S. */
struct context {
  uint64 ra;
  uint64 sp;
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

struct thread {
  struct context ctx;
  char stack[STACK_SIZE] __attribute__((aligned(16)));
  int state;
};

struct thread all_thread[MAX_THREAD];
struct thread *current_thread;

extern void thread_switch(struct context *old, struct context *new);

void
thread_init(void)
{
  current_thread = &all_thread[0];
  current_thread->state = RUNNING;
}

void
thread_schedule(void)
{
  struct thread *t, *next_thread;

  next_thread = 0;
  t = current_thread + 1;
  for (int i = 0; i < MAX_THREAD; i++) {
    if (t >= all_thread + MAX_THREAD)
      t = all_thread;
    if (t->state == RUNNABLE) {
      next_thread = t;
      break;
    }
    t = t + 1;
  }

  if (next_thread == 0) {
    printf("thread_schedule: no runnable threads\n");
    exit(-1);
  }

  if (current_thread != next_thread) {
    struct thread *old = current_thread;
    next_thread->state = RUNNING;
    current_thread = next_thread;
    thread_switch(&old->ctx, &next_thread->ctx);
  }
}

void
thread_create(void (*func)(void))
{
  struct thread *t;

  for (t = all_thread; t < all_thread + MAX_THREAD; t++) {
    if (t->state == FREE)
      break;
  }
  if (t == all_thread + MAX_THREAD) {
    fprintf(2, "thread_create: no free thread slots\n");
    exit(1);
  }
  t->state = RUNNABLE;
  t->ctx.ra = (uint64)func;
  t->ctx.sp = (uint64)(t->stack + STACK_SIZE);
}

void
thread_yield(void)
{
  current_thread->state = RUNNABLE;
  thread_schedule();
}

volatile int a_started, b_started, c_started;
volatile int a_n, b_n, c_n;

void
thread_a(void)
{
  printf("thread_a started\n");
  a_started = 1;
  while (b_started == 0 || c_started == 0)
    thread_yield();

  for (int i = 0; i < 100; i++) {
    printf("thread_a %d\n", i);
    a_n += 1;
    thread_yield();
  }
  printf("thread_a: exit after %d\n", a_n);
  current_thread->state = FREE;
  thread_schedule();
}

void
thread_b(void)
{
  printf("thread_b started\n");
  b_started = 1;
  while (a_started == 0 || c_started == 0)
    thread_yield();

  for (int i = 0; i < 100; i++) {
    printf("thread_b %d\n", i);
    b_n += 1;
    thread_yield();
  }
  printf("thread_b: exit after %d\n", b_n);
  current_thread->state = FREE;
  thread_schedule();
}

void
thread_c(void)
{
  printf("thread_c started\n");
  c_started = 1;
  while (a_started == 0 || b_started == 0)
    thread_yield();

  for (int i = 0; i < 100; i++) {
    printf("thread_c %d\n", i);
    c_n += 1;
    thread_yield();
  }
  printf("thread_c: exit after %d\n", c_n);
  current_thread->state = FREE;
  thread_schedule();
}

int
main(void)
{
  a_started = b_started = c_started = 0;
  a_n = b_n = c_n = 0;
  thread_init();
  thread_create(thread_a);
  thread_create(thread_b);
  thread_create(thread_c);
  current_thread->state = FREE;
  thread_schedule();
  exit(0);
}
