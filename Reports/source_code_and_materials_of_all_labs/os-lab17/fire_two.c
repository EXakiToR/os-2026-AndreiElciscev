/* fire.c - two threads of one process share the same memory.
 * Both add to the same counter, a million times each, WITHOUT a lock.
 * Build: gcc -Wall -Wextra -pthread -o fire fire.c   Run: ./fire
 */
#include <stdio.h>
#include <pthread.h>

#define N 1000000
long counter = 0;              /* one variable, shared by both threads */

void *work(void *arg) {
    long id = (long)arg;
    printf("thread %ld sees counter = %ld\n", id, counter);
    for (int i = 0; i < N; i++)
        counter++;             /* NOT atomic: read, add, write */
    return NULL;
}

int main(void) {
    pthread_t a, b;
    pthread_create(&a, NULL, work, (void *)1);
    pthread_create(&b, NULL, work, (void *)2);
    pthread_join(a, NULL);
    pthread_join(b, NULL);
    printf("expected: %d\n", 2 * N);
    printf("counter:  %ld   (lost updates: the race)\n", counter);
    return 0;
}
