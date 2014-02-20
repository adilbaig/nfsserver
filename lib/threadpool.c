#include "threadpool.h"

static pthread_cond_t queue_ready = PTHREAD_COND_INITIALIZER;
static pthread_mutex_t queue_lock = PTHREAD_MUTEX_INITIALIZER;

static pthread_t thread_ids[THREADPOOL_MAX_THREADS_NUM];
static struct thread_pool_queue *qp;
size_t num_threads = 0;

static void init_queue();
static struct thread_pool_task* threadpool_queue_task_shift();
static void * thr_fn(void *arg);

int Pthread_create(pthread_t *thread, const pthread_attr_t *attr, void
        *(*start_routine) (void *), void *arg) {
    int e = pthread_create(thread, attr, *start_routine, arg);
    if (e != 0)
        unix_error("Could not create thread");
    return e;
}

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

    printf("Created thread: %lu \n", (unsigned long)pthread_self());

    while (1) {
        struct thread_pool_task *tp = threadpool_queue_task_shift();
        if (!tp)
            continue;
        printf("%lu: Executing %d \n", (unsigned long)pthread_self(), *(int *)tp->arg);

        tp->routine(tp->arg);
        free(tp);
    }

    return NULL;
}

static void init_queue() {

    qp = (struct thread_pool_queue *) malloc(sizeof(struct thread_pool_queue));
    if (qp == NULL) {
        unix_error("Could not allocate memory for queue \n");
    }
    qp->headp = NULL;
    qp->tailp = NULL;
}

void threadpool_queue_task_push(void *(*routine)(void *), void *arg) {

    pthread_mutex_lock(&queue_lock);

    struct thread_pool_task *tp = malloc(sizeof(struct thread_pool_task));
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

static struct thread_pool_task* threadpool_queue_task_shift() {

    pthread_mutex_lock(&queue_lock);
    if (!qp->headp && !qp->tailp) {
        pthread_cond_wait(&queue_ready, &queue_lock);
        if (!qp->headp && !qp->tailp) {
            pthread_mutex_unlock(&queue_lock);
            return NULL;
        }
    }

    struct thread_pool_task *tp = qp->headp;
    qp->headp = tp->nextp;

    if (!qp->headp) {
        qp->tailp = NULL;
    }

    pthread_mutex_unlock(&queue_lock);
    return tp;
}
