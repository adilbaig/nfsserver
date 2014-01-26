#include <semaphore.h>
#include "threadpool.h"

pthread_cond_t queue_ready = PTHREAD_COND_INITIALIZER;
pthread_mutex_t queue_lock = PTHREAD_MUTEX_INITIALIZER;

struct task {
    struct task *nextp;
    void *(*routine)(void *);
    void *arg;
};

struct queue {
    struct task *headp;
    struct task *tailp;
};


static pthread_t thread_ids[THREADPOOL_MAX_THREADS_NUM];
static struct queue *qp;
size_t num_threads = 0;

static void init_queue();
static struct task* threadpool_queue_task_shift();
static void * thr_fn(void *arg);
void print_queue();


int thread_pool_init(size_t n_threads) {

    size_t i = 0;
    num_threads = n_threads;

    init_queue();
    while (i < num_threads) {
        pthread_t tid;
        Pthread_create(&tid, NULL, thr_fn, NULL);
        thread_ids[i] = tid;
        i++;
    }

    return 1;
}

void thread_pool_shutdown() {

    int i;
    for (i = 0; i < num_threads; i++) {
        pthread_cancel(thread_ids[i]);
    }
    free(qp);
    pthread_cond_destroy(&queue_ready);
    pthread_mutex_destroy(&queue_lock);
}

static void * thr_fn(void *arg) {

    printf("Created thread: %lu \n", pthread_self());

    while (1) {
        struct task *tp = threadpool_queue_task_shift();
        if (!tp)
            continue;
        printf("%lu: Executing %d \n", pthread_self(), *(int *)tp->arg);

        tp->routine(tp->arg);
        free(tp);
    }

    return NULL;
}

static void init_queue() {

    qp = (struct queue *) malloc(sizeof(struct queue));
    if (qp == NULL) {
        unix_error("Could not allocate memory for queue \n");
    }
    qp->headp = NULL;
    qp->tailp = NULL;
}

void threadpool_queue_task_push(void *(*routine)(void *), void *arg) {

    pthread_mutex_lock(&queue_lock);

    struct task *tp = (struct task *)malloc(sizeof(struct task));
    if (!tp) {
        unix_error("Failed to allocate space for an task");
    }
    tp->nextp = NULL;
    tp->routine = routine;
    tp->arg = arg;
    if (!qp->headp && !qp->tailp) {
        qp->headp = tp;
        qp->tailp = tp;
    } else {
        qp->tailp->nextp = tp;
        qp->tailp = tp;
    }

    pthread_mutex_unlock(&queue_lock);
    pthread_cond_signal(&queue_ready);
    printf("Pushed fd: %d \n", (*(int *)arg));
}

static struct task* threadpool_queue_task_shift() {

    pthread_mutex_lock(&queue_lock);
    if (!qp->headp && !qp->tailp) {
        pthread_cond_wait(&queue_ready, &queue_lock);
        if (!qp->headp && !qp->tailp) {
            pthread_mutex_unlock(&queue_lock);
            return NULL;
        }
    }

    struct task *tp = qp->headp;
    qp->headp = tp->nextp;

    if (!qp->headp) {
        qp->tailp = NULL;
    }

    pthread_mutex_unlock(&queue_lock);
    return tp;
}

void print_queue() {

    struct task *tp = qp->headp;
    while(tp) {
        printf("q> fd: %d \n", (*(int *)tp->arg));
        tp = tp->nextp;
    }
}
