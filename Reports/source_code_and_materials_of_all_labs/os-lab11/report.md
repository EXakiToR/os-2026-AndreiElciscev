# Lab 11 – The Race and the Semaphore

Andrei Elciscev, FAF-243  
Linux VM: /home/user/os-lab11  
Build flags: `gcc -Wall -pthread` (default optimization, no `-O2`)

## Part 1 – Observe

1. **What are the three steps of `counter++`?** Load the shared value, add one to the loaded value, and store the result back. This read–modify–write sequence is not atomic.
2. **How can two threads lose an addition?** If the counter starts at 10, both threads can load 10 before either stores. Both calculate 11 and store 11. The final value is 11 instead of 12.
3. **Why do results vary?** Scheduling and execution overlap vary across runs, changing how the loads and stores interleave and how many updates get overwritten. The conflicting unsynchronized accesses are a C data race with undefined behavior. Different runs can give different results, but distinct results are not guaranteed.

The three observed counts were 1,675,785, 1,925,845, and 1,205,596. Each worker attempted 1,000,000 increments.

## Part 2 – Observe

1. **Did mutex and sem reach 2,000,000?** Yes. Each program returned 2,000,000 in all three requested trials and again in all three timed trials. The mutex and the semaphore initialized to one each allow one thread in the critical section at a time.
2. **What happened with `sem_init(&s, 0, 2)`?** The counts were 1,999,341, 1,947,310, and 1,994,467. With two permits, both workers can pass `sem_wait` at the same time, so the counter increments race again. The submitted `sem.c` starts at one.
3. **Why is the locked version slower?** Each increment requires an acquire and a release. Contention can add cache-coherence traffic and can cause threads to wait and wake. The median wall times for race, mutex, and semaphore were 0.015 s, 0.058 s, 0.072 s, respectively. These are measurements of this VM run.

## Part 3 – Observe

1. **What did B read with the semaphore initially at one?** B printed an empty string before A wrote. The initial permit allowed B through `sem_wait` without a post, and the global character array initially contained zeros. Without the wait for A's post, the read and write have no synchronization ordering. The empty string is the observed outcome.
2. **What does starting at zero mean?** A wait cannot complete until a post supplies a permit. With the original zero restored, A wrote and posted before B read `hello`.

## Part 4 – Observe

1. **What was the highest occupancy and what enforces it?** The maximum was three. `sem_init(&places, 0, PLACES)` supplies three permits and `sem_wait(&places)` requires a permit before a car enters. Each car returns its permit with `sem_post(&places)` after leaving. The mutex protects the occupancy update and its corresponding print.
2. **What happened at `PLACES=1`?** The semaphore acted as a binary semaphore for the parking slot. Cars parked one at a time, and the highest occupancy was one. The submitted `parking.c` uses three places.

All six cars entered and left once in both runs, and each run ended with zero cars inside. Exact commands and output are in `evidence.txt`.
