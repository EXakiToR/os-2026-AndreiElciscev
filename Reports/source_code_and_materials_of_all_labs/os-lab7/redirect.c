#include <stdio.h> 
#include <stdlib.h> 
#include <fcntl.h> 
#include <unistd.h> 
#include <sys/wait.h> 
 
/* does the same as the shell command:   ls -l > out.txt   */ 
int main(void) 
{ 
    pid_t pid = fork(); 
    if (pid == 0) { 
        close(1);                                /* descriptor 1 is now free */ 
        int fd = open("out.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644); 
        if (fd < 0) { perror("open"); exit(1); } 
        fprintf(stderr, "open gave descriptor %d\n", fd); 
        execlp("ls", "ls", "-l", (char *)NULL);  /* ls writes to 1, as always */ 
        perror("exec"); 
        exit(127); 
    } 
    waitpid(pid, NULL, 0); 
    printf("done: the listing is in out.txt\n"); 
    return 0; 
}
