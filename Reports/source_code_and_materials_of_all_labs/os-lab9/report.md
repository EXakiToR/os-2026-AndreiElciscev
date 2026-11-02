# Laboratory Work 9 - Watching System Calls with strace

Student: Andrei Elciscev, FAF-243
Working directory: /home/user/os-lab9
Environment: Linux Mint 22.3; Linux 6.14.0-37-generic; GCC 13.3.0; strace 6.8.

## Part 1 - Observe
**Which system call does ls use to print to the screen? To which descriptor does it write?**

`write`, to descriptor 1 (standard output). In the initial trace it wrote 113 bytes.
Here stdout was captured to an evidence file; interactively it normally points to the terminal.

### Rough call count
`strace -f -c -e trace=openat,read,write,close ls -l` reported 38 selected calls
(10 openat, 11 read, 16 close, 1 write). The unfiltered `strace -f -c ls -l`
reported 122 calls, including 2 getdents64 calls. Counts vary with environment
and directory contents; the later listings include the compiled program.

## Part 2 - Observe
**The descriptor returned by the first openat is 3, not 0, 1 or 2. Why are those three already taken?**

They are inherited standard descriptors: 0 stdin, 1 stdout, 2 stderr.
After loader setup, 3 is the lowest available descriptor. Closing it allows
the second open of note.txt to reuse 3.

### Execution result
Compilation with gcc -Wall -Wextra produced no diagnostics. The program wrote
14 bytes and read back 14 bytes: `hello, kernel` followed by a newline.
Exact-content and byte-count checks both passed.
The libc open wrapper appeared as openat in the trace.

## Part 3 - Supplementary call-count experiment
The supplied PDF has two pages and lists Part 3 in the overview, but includes no
Part 3 instructions or Observe questions. To cover its stated goal, I ran
`strace -c -e trace=write ./syscalls`: 2 writes, one for 14 bytes to note.txt
and one for 80 bytes of buffered stdout messages. Non-terminal capture causes
the three printf calls to be combined into one stdout write. Terminal counts can differ.
The four-call summary contains 12 calls (4 openat, 2 read, 2 write, 4 close),
including loader setup. Six of these operate specifically on note.txt.

## Evidence
All captured commands and their output are in evidence/*.txt.
The clean trace is evidence/syscalls-clean.trace. Source is syscalls.c.
The original lab program is retained; production code should also check the
second open, read failures, and partial writes.
