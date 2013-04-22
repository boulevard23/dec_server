#include <stdio.h>
#include <strings.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <time.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <dirent.h>

#include "server.h"
#include "io.h"

pthread_mutex_t mutex;
sem_t sem;
char error[128];
char response[128];

/*
 *  start the server, and listen to the given port
 */
int initServer(int port) {
  int sock_id;
  struct sockaddr_in saddr;
  sock_id = socket(AF_INET, SOCK_STREAM, 0);
  if (sock_id == -1 ) {
    perr("Create socket error!");
  }
  bzero(&saddr, sizeof(saddr));
  saddr.sin_family = AF_INET;
  saddr.sin_port = htons(port);
  saddr.sin_addr.s_addr = INADDR_ANY;
  if (bind(sock_id, (struct sockaddr *) &saddr, sizeof(saddr)) != 0) {
    perr("Server binding error!");
  }
  if (listen(sock_id, 5)) {
    perr("Server listen error!");
  }
  printf("Listening to the port: %d \n", port);
  return sock_id;
}

void handleRequest(void* arguments) {
  Args* args = arguments;
  int clientId = args->sock_fd;
  void* nodeList = args->nodeList;
  char* path = args->logPath;
  char* clientName = args->clientName;
  int logFlg = args->logFlg;
  char buffer[128];
  bzero(buffer, sizeof(buffer));
  List* cmdList[48];
  int size;
  int i;
  FILE* logFile;


  while (read(clientId, buffer, sizeof(buffer))) {
    if (logFlg == 1) {
      logFile = fopen(path, "a+");
      fprintf(logFile, "%s", buffer);
      fclose(logFile);
    }
    bzero(error, sizeof(error));
    bzero(response, sizeof(response));
    memset(cmdList, 0, sizeof(cmdList));
    size = AnalyzeInput(cmdList, buffer);
    printf("size: %d\n", size);
    if (size > 0) {
      for (i = 0; i < size; i++) {
        pthread_mutex_lock(&mutex);
        ExecuteCmd(cmdList[i], nodeList, clientName);
        /*printf("traverse\n");*/
        /*Traverse(nodeList);*/
        pthread_mutex_unlock(&mutex);
        write(clientId, error, strlen(error));
        write(clientId, response, strlen(response));
        if (logFlg == 1) {
          logFile = fopen(path, "a+");
          fprintf(logFile, "%s", error);
          fprintf(logFile, "%s", response);
          fclose(logFile);
        }
        bzero(error, sizeof(error));
        bzero(response, sizeof(response));
      }
    } else if (size == -1 || size == -3) {
      write(clientId, error, strlen(error));
      if (logFlg == 1) {
        logFile = fopen(path, "a+");
        fprintf(logFile, "%s", error);
        fclose(logFile);
      }
    } else {
      continue;
    }
    bzero(buffer, sizeof(buffer));
  }
  close(clientId);
}
