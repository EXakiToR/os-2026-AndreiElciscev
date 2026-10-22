#include "kernel/types.h" 
#include "user/user.h" 
 
// the three steps of the shell, inside xv6:  echo hello from the child 
int 
main(int argc, char *argv[]) 
{ 
  printf("parent: my pid is %d\n", getpid()); 
 
  int pid = fork();                     // step 1: copy myself 
  if(pid < 0){ 
    printf("fork failed\n"); 
    exit(1); 
  } 
 
  if(pid == 0){                         // only the child runs this 
    printf("child: my pid is %d\n", getpid()); 
    char *args[] = { "ls", 0 }; 
    exec("nosuchprog", args);                   // step 2: become ls 
    printf("exec failed\n");            // runs only if exec failed 
    exit(1); 
  } 
 
  int status; 
  wait(&status);                        // step 3: wait for the child 
  printf("parent: child %d ended with status %d\n", pid, status); 
  exit(0); 
}
