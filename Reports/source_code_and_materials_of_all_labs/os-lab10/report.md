# Lab 10 - Add Your Own System Call: trace

Student: Andrei Elciscev, FAF-243
VM working directory: /home/user/os-lab10
xv6 tree: /home/user/os-lab10/xv6-riscv
Branch: codex/lab10-trace
Implementation commit: 4f95820916485f6480ddb2c27eeca7f5683f8faf

## Part 1
Added SYS_trace 23 (22 is sync), user declaration, usys.pl entry,
user/trace.c, and a Makefile UPROGS entry. The generated stub loads a7,
executes ecall, and returns. Check 1 produced:
3 trace: unknown sys call 23
trace: failed
The check passed before implementing the handler.

## Part 2
Added per-process trace_mask, sys_trace, dispatch entry, syscall name table,
and printk after the handler returns. Children inherit the mask in kfork:
np->trace_mask = p->trace_mask;
The new field is explicitly initialized to zero in allocproc.
Check 2 produced read return values 1023, 965, 453, 0. The positive reads
sum to 2441, the exact README size.

## Part 3 - Observe answers (also saved as notes.txt)
1. Why only read with mask 32, not open or close?
32 = 1 << 5 selects SYS_read (5). Bits 15 (open) and 21 (close) are not set.
The other calls still execute, but they are not printed.

2. Why do different PIDs print lines in forkforkfork? Which line makes it possible?
np->trace_mask = p->trace_mask; copies the parent's mask to its child.
Descendants therefore retain the fork tracing mask and print their own calls.

3. Why is the trace between hi and the newline? How many writes does echo make?
echo hi makes two writes: write(1, "hi", 2), then write(1, "\n", 1).
The first trace line is printed after the first call returns, before the next
write sends the newline. The observed return values are 2 and 1.

4. Which mask selects open (15) and close (21) only?
(1 << 15) | (1 << 21) = 32768 + 2097152 = 2129920.
Verified with trace 2129920 grep hello README: only open and close printed.

## Tests and results
- Baseline, Part 1, and final builds and boots passed.
- trace 32 grep hello README: exact required read sequence.
- trace 2147483647 grep hello README: trace, exec, open, read, close printed.
- trace 2 usertests forkforkfork: OK; ALL TESTS PASSED.
- trace 65536 echo hi: two write trace lines, return values 2 and 1.
- trace 2129920 grep hello README: open and close only.
- trace 0 echo untraced: ordinary output without tracing.
- trace: usage error shown.
- trace 32 missingcommand: execution error shown.
- git diff --check passed; all nine implementation files committed.

Evidence: evidence/*.txt. Original coursework tree was preserved unchanged.
Git bundle: xv6-lab10.bundle (complete committed source history and lab branch).
The inherited untracked args executable is an earlier coursework artifact.
