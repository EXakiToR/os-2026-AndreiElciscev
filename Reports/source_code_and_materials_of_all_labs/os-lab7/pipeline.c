#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <sys/wait.h> 
 
/* does the same as the shell command:   ls /etc | wc -l   */ 
int main(void) 
{ 
    int p[2]; 
    if (pipe(p) < 0) { perror("pipe"); return 1; } 
 
    if (fork() == 0) {                           /* ls: its 1 becomes p[1] */ 
        close(1); 
        dup(p[1]);                               /* smallest free number: 1 */ 
        close(p[0]); 
        close(p[1]); 
        execlp("ls", "ls", "/etc", (char *)NULL); 
        perror("exec ls"); 
        exit(127); 
    } 
 
    if (fork() == 0) {                           /* wc: its 0 becomes p[0] */ 
        close(0); 
        dup(p[0]);                               /* smallest free number: 0 */ 
        close(p[0]); 
        close(p[1]); 
        execlp("wc", "wc", "-l", (char *)NULL); 
        perror("exec wc"); 
        exit(127); 
    } 
 
    close(p[0]);                                 /* the parent uses neither end */ 
    /* close(p[1]); */  
    wait(NULL); 
    wait(NULL); 
    return 0; 
}
