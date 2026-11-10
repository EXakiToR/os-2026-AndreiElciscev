# Lab 12 - Producer, Consumer and the Other Tools

Andrei Elciscev, FAF-243

VM folder: /home/user/os-lab12

## Part 1 - Producer and Consumer

1. **What stops the producer at four items?** `sem_wait(&empty)` waits for a free slot. The producer consumes one of four permits per insertion, and the consumer returns a permit with `sem_post(&empty)` after removing an item.
2. **Who waits at the start?** The consumer must wait at `sem_wait(&full)` if it reaches that call before the first insertion. The initial value of `full` is zero. The producer can proceed immediately because `empty` starts at four.
3. **What happens if the producer locks before waiting?** The altered program filled four slots and timed out with exit code 124. The producer held the mutex while waiting for an empty slot. The consumer needed that mutex to remove an item and release a slot. The submitted program waits for a slot before acquiring the mutex.

## Part 2 - Condition Variable

1. **Which variable remembers that the message is ready?** `ready` stores the predicate and the mutex protects it. `changed` is the condition variable used to notify a waiter.
2. **Did replacing while with if work here?** Yes. The altered run printed the same three lines, ending with the reader printing hello. The writer set `ready = 1` before signaling, and this run had no early wakeup.
3. **Why check the predicate in a loop?** A condition wait can wake spuriously while `ready` is still zero. The reader reacquires the mutex and checks the predicate again before reading. The submitted program uses the loop.

## Part 3 - Readers and Writers

1. **Which lines let readers share the room?** The first reader executes `if (++readers == 1) sem_wait(&room);`. The last reader executes `if (--readers == 0) sem_post(&room);`. The group holds the room while any reader is inside, so each writer must wait for exclusive access.
2. **Why does the reader counter need its own lock?** Without `rlock`, updates can race and lose counts. Two readers could both act as the first reader, or a miscount could release the room while readers remain or keep it locked after they leave. The submitted code also takes the diagnostic counter snapshot under `rlock` to avoid an unsynchronized read.
3. **What if readers keep arriving?** If the active reader count never reaches zero, a waiting writer may never acquire the room. This is writer starvation.

## Part 4 - Deadlock

1. **What is the circular wait?** Thread one holds A and waits for B. Thread two holds B and waits for A. The wait-and-ownership cycle is shown below.

   `Thread one -> mutex B -> thread two -> mutex A -> thread one`

2. **Do the ten runs without sleeps always hang?** No. 3 timed out (trials 2, 3, 9) and 7 completed. Removing the sleeps makes the bug depend more on scheduling. A successful run can hide the bug during testing.
3. **Why does a consistent acquisition order prevent this deadlock?** When both threads acquire A before B, the thread waiting for A holds neither mutex, so there is no circular wait.

All eight items were consumed in FIFO order. Buffer occupancy peaked at four and ended at zero. Both mutexes use A-then-B order in the submitted deadlock.c. Commands and actual results are in evidence.txt.
