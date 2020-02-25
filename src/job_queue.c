#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "job_queue.h"

pthread_mutex_t queue_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t dataAvailable = PTHREAD_COND_INITIALIZER;
pthread_cond_t spaceAvailable = PTHREAD_COND_INITIALIZER;
pthread_cond_t popDone = PTHREAD_COND_INITIALIZER;

int job_queue_init(struct job_queue *job_queue, int capacity) {
  job_queue->capacity = capacity;
  job_queue->data = malloc(sizeof(void *) * capacity);
  job_queue->popIndex = 0;
  job_queue->pushIndex = 0;
  job_queue->capacityLeft = capacity;
  job_queue->pushingDone = 0;
  job_queue->popDone = 0;
  return 0;
}

int job_queue_destroy(struct job_queue *job_queue) {
  assert(pthread_mutex_lock(&queue_mutex) == 0);
  assert(pthread_cond_broadcast(&dataAvailable) == 0);
  job_queue->pushingDone = 1;
  while (!(job_queue->popDone)) {
    assert(pthread_cond_wait(&popDone, &queue_mutex) == 0);
  }
  free(job_queue->data);
  assert(pthread_mutex_unlock(&queue_mutex) == 0);
  assert(pthread_cond_broadcast(&dataAvailable) == 0);
  return 0;
}

int job_queue_push(struct job_queue *job_queue, void *data) {
  assert(pthread_mutex_lock(&queue_mutex) == 0);
  while (job_queue->capacityLeft == 0) {
    assert(pthread_cond_wait(&spaceAvailable, &queue_mutex) == 0);
  }
  if (job_queue->pushIndex == job_queue->capacity) {
    job_queue->pushIndex = 0;
  }
  job_queue->data[job_queue->pushIndex] = data;
  job_queue->pushIndex++;
  job_queue->capacityLeft--;
  assert(pthread_mutex_unlock(&queue_mutex) == 0);
  assert(pthread_cond_broadcast(&dataAvailable) == 0);
  return 0;
}

int job_queue_pop(struct job_queue *job_queue, void **data) {
  assert(pthread_mutex_lock(&queue_mutex) == 0);
  while (job_queue->capacity == job_queue->capacityLeft) {
    // Subsequent threads seeing that popping is finished
    if (job_queue->popDone == 1) {
      assert(pthread_mutex_unlock(&queue_mutex) == 0);
      assert(pthread_cond_broadcast(&dataAvailable) == 0);
      return -1;
    }
    // First thread seeing that popping is finished
    if (job_queue->pushingDone) {
      job_queue->popDone = 1;
      assert(pthread_mutex_unlock(&queue_mutex) == 0);
      assert(pthread_cond_broadcast(&popDone) == 0);
      return -1;
    }
    assert(pthread_cond_wait(&dataAvailable, &queue_mutex) == 0);
  }
  if (job_queue->popIndex == job_queue->capacity) {
    job_queue->popIndex = 0;
  }
  *data = job_queue->data[job_queue->popIndex];
  job_queue->popIndex++;
  job_queue->capacityLeft++;
  assert(pthread_mutex_unlock(&queue_mutex) == 0);
  assert(pthread_cond_broadcast(&spaceAvailable) == 0);
  return 0;
}
