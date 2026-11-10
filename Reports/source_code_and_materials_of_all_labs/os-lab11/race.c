/* race.c - two threads add to the same counter, with no protection.
 * Build:  gcc -Wall -pthread -o race race.c      (no -O2: keep counter++ in memory)
 * Run:    ./race   (several times)
 */
#include <stdio.h>
#include <pthread.h>

#define N 1000000

long counter = 0;                 /* shared by both threads */

void *add(void *arg) {
    for (int i = 0; i < N; i++)
        counter++;                /* read, add 1, write back: three steps */
    return NULL;
}

int main(void) {
    pthread_t a, b;
    pthread_create(&a, NULL, add, NULL);
    pthread_create(&b, NULL, add, NULL);
    pthread_join(a, NULL);
    pthread_join(b, NULL);
    printf("counter = %ld (expected %d)\n", counter, 2 * N);
    return 0;
}
