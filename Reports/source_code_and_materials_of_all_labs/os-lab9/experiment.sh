#!/bin/bash
set -eu
cd /home/user/os-lab9
export LC_ALL=C
mkdir -p evidence
run() {
  local name="$1" command="$2"
  { printf '$ %s\n' "$command"; bash -o pipefail -c "$command"; } > "evidence/$name.txt" 2>&1
  cat "evidence/$name.txt"
}
run 00_environment 'pwd; uname -sr; gcc --version | head -1; strace --version | head -1'
run 01_ls 'strace -f -e trace=openat,read,write,close ls -l'
cat > syscalls.c <<'EOF'
/* syscalls.c - open, write, read and close. Lab 9 supplied program. */
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

int main(void) {
    const char *msg = "hello, kernel\n";
    int fd = open("note.txt", O_CREAT | O_WRONLY | O_TRUNC, 0644);
    if (fd < 0) { perror("open"); return 1; }
    printf("open returned descriptor %d\n", fd);
    ssize_t n = write(fd, msg, strlen(msg));
    printf("write sent %zd bytes\n", n);
    close(fd);
    fd = open("note.txt", O_RDONLY);
    char buf[64];
    n = read(fd, buf, sizeof(buf) - 1);
    buf[n] = '\0';
    printf("read got %zd bytes: %s", n, buf);
    close(fd);
    return 0;
}
EOF
run 02_source 'cat syscalls.c'
run 03_build_run 'gcc -Wall -Wextra -o syscalls syscalls.c; ./syscalls; wc -c note.txt; cat note.txt'
run 04_program_trace 'strace -e trace=openat,write,read,close ./syscalls'
run 05_ls_filtered_count 'strace -f -c -e trace=openat,read,write,close ls -l'
run 06_ls_all_count 'strace -f -c ls -l'
run 07_write_count 'strace -c -e trace=write ./syscalls'
run 08_program_count 'strace -c -e trace=openat,write,read,close ./syscalls'
run 09_verify 'test "$(wc -c < note.txt)" -eq 14 && printf "PASS: note.txt contains 14 bytes\n"; printf "hello, kernel\n" | cmp - note.txt && printf "PASS: file content matches the written message\n"'
