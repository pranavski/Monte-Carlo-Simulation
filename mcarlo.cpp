#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <pthread.h>
#include <semaphore.h>

//random double between 0 and 1
double random_double() {
    return (double)rand() / (RAND_MAX + 1.0);
}
void *worker_thread(void *arg);
pthread_mutex_t work_mutex;
// keep track of points in circle
unsigned long long num_points_in_circle = 0;
unsigned long long num_total_points = 0;
int time_to_exit = 0;

int main(int argc, char* argv[]) {
    int res;
    pthread_t worker_threads[2];
    void *thread_result;
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <number of points>\n", argv[0]);
        return 1;
    }
    unsigned long long num_points;
    if (sscanf(argv[1], "%llu", &num_points) != 1) {
        fprintf(stderr, "Invalid number of points: %s\n", argv[1]);
        return 1;
    }
    //initialize mutex
    res = pthread_mutex_init(&work_mutex, NULL);
    if (res != 0) {
        perror("Mutex initialization failed");
        exit(EXIT_FAILURE);
    }
    unsigned long long points_per_thread = num_points / 2;
    //worker threads
    res = pthread_create(&worker_threads[0], NULL, worker_thread, &points_per_thread);
    if (res != 0) {
        perror("Thread creation failed");
        exit(EXIT_FAILURE);
    }
    res = pthread_create(&worker_threads[1], NULL, worker_thread, &points_per_thread);
    if (res != 0) {
        perror("Thread creation failed");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < 2; ++i) {
        res = pthread_join(worker_threads[i], &thread_result);
        if (res != 0) {
            perror("Thread join failed");
            exit(EXIT_FAILURE);
        }
    }
    double pi_estimate = 4.0 * (double)num_points_in_circle / num_total_points;
    printf("Estimated value of pi: %.15f\n", pi_estimate);
    pthread_mutex_destroy(&work_mutex);
    return 0;
}
void *worker_thread(void *arg) {   
    unsigned long long num_points = *(unsigned long long*)arg;
    unsigned long long hit_count = 0;
    for (unsigned long long i = 0; i < num_points; ++i) {
        double x = random_double() * 2.0 - 1.0;
        double y = random_double() * 2.0 - 1.0;
        if (sqrt(x * x + y * y) < 1.0)
            ++hit_count;
    }
    pthread_mutex_lock(&work_mutex);
    num_points_in_circle += hit_count;
    num_total_points += num_points;
    pthread_exit(0);
}