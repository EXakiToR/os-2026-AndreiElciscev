/* fire.c - four threads of one process share the same memory.
 * All add to the same counter, a million times each, WITHOUT a lock.
 * Build: gcc -Wall -Wextra -pthread -o fire fire.c   Run: ./fire
 */
#include <stdio.h>
#include <pthread.h>

#define N 1000000
long counter = 0;              /* one variable, shared by all four threads */

void *work(void *arg) {
    long id = (long)arg;
    printf("thread %ld sees counter = %ld\n", id, counter);
    for (int i = 0; i < N; i++)
        counter++;             /* NOT atomic: read, add, write */
    return NULL;
}

int main(void) {
    pthread_t a, b, c, d;
    pthread_create(&a, NULL, work, (void *)1);
    pthread_create(&b, NULL, work, (void *)2);
    pthread_create(&c, NULL, work, (void *)3);
    pthread_create(&d, NULL, work, (void *)4);
    pthread_join(a, NULL);
    pthread_join(b, NULL);
    pthread_join(c, NULL);
    pthread_join(d, NULL);
    printf("expected: %d\n", 4 * N);
    printf("counter:  %ld   (lost updates: the race)\n", counter);
    return 0;
}
