#include <pthread.h>
#include <semaphore.h>

#define perr(msg) { perror(msg); exit(1); }

extern pthread_mutex_t mutex;
extern sem_t sem;


/* Individual job struct. Accept function, arguments and job size. */
typedef struct JobStruct {
  void *(*function)(void *arg);
  void *arg;
  struct JobStruct *next;
  struct JobStruct *prev;
}Job;

/* Queue struct. */
typedef struct QueueStruct {
  Job *front;
  Job *rear;
  int currentSize;
}Queue;

/* Thread pool struct. */
typedef struct PoolStruct {
  pthread_t *threads;
  int threadsNum;
  Queue *queue;
}ThreadPool;

/* Arguments struct. Store the args which will passed into every job. */
typedef struct ArgStruct {
  int sock_id;
  int sock_fd;
  char* logPath;
  int logFlg;
  void* nodeList;
  char* clientName;
}Args;

/* start the server */
int initServer();

void handleRequest(void *);
