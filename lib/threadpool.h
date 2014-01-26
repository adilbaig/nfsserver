#include "../lib/common.h"

#define THREADPOOL_MAX_THREADS_NUM 1000

int thread_pool_init(size_t num_threads);
void threadpool_queue_task_push(void *(*routine)(void *), void *arg);
void thread_pool_shutdown();

