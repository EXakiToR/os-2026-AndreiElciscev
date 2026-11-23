# Lab 17 - Threads Share Memory, and Race

Andrei Elciscev, FAF-243

VM folder: /home/user/os-lab17

## Part 1 - Shared and racing

The supplied two-thread program is retained as fire_two.c. It was built with
gcc -Wall -Wextra -pthread -o fire_two fire_two.c and run three times.

| Trial | Expected | Observed | Lost updates |
| --- | ---: | ---: | ---: |
| 1 | 2000000 | 1207310 | 792690 |
| 2 | 2000000 | 1321397 | 678603 |
| 3 | 2000000 | 1821131 | 178869 |

### Observe 1 - What does thread 2's non-zero initial counter show?

In trial 3, thread 2 saw counter = 195948 before starting its loop. It saw the
increments of thread 1 because both access the same global variable in one
process address space. Both initial reads were zero in the first two trials,
demonstrating that the relative start timing can differ.

### Observe 2 - Why are updates lost and results different?

counter++ reads the counter, adds one, and writes the result. If both threads read
10 before either stores, both may compute 11 and store 11. Two attempted increments
then change the counter by one. Scheduling and overlap change which updates
overwrite others. These unsynchronized conflicting accesses are a C data race
with undefined behavior. The observed losses illustrate the problem, but the
language does not guarantee a particular value, a different result on every
execution, or a result strictly below the expected count in every execution.

## Part 2 - Four threads

fire.c creates and joins four workers and changes the expected total to
4 * N = 4000000. Each worker still attempts one million increments.

| Trial | Expected | Observed | Lost updates |
| --- | ---: | ---: | ---: |
| 1 | 4000000 | 2417179 | 1582821 |
| 2 | 4000000 | 2087047 | 1912953 |
| 3 | 4000000 | 1629452 | 2370548 |

The mean absolute loss rose from 550054 to 1955441
updates. The mean lost fraction rose from 27.50% to 48.89%.
The VM exposes three CPUs. More workers increased loss in these trials, but the
amount lost need not increase monotonically for every execution.

### Observe 3 - One idea to prevent lost updates

Protect the entire read-add-write operation with one shared mutex so only one
thread can increment the counter at a time.

The handout asks for a proposed fix. The submitted programs retain the race.
Commands and actual output are in evidence.txt.
