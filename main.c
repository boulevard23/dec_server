#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>

#include "server.h"
#include "io.h"


int main(int argc, char *argv[]) {
  static const char help[] =
    "\nDistributed Event Coordination System - Server:\n\n"
    "   -h              - Print this usage summary with all options.\n\n"
    "   -p port-number  - Listen on the given port. The default port is 9090.\n\n"
    "   -l file         - Log all requests and responses to the given file. If not, print all to stdout.\n\n";

  int c;
  int sock_id, sock_fd;
  int portNo = 9090;
  pthread_t threads[20];
  int logFlg = 0;
  char* path = NULL;
  int threadsCount = 0;

  opterr = 0;

  while((c = getopt(argc, argv, "hp:l:")) != -1) {
    switch(c) {
      case 'h':
        printf("%s", help);
        exit(0);
        break;
      case 'p':
        portNo = atoi(optarg);
        if (portNo == 0 && optarg != '0') {
          perr("option -p argument type error! \n");
        }
        break;
      case 'l':
        logFlg = 1;
        path = optarg;
        break;
      case '?':
        if (optopt == 'l') {
          perr("option -l needs an argument. \n");
        } else if (optopt == 'p') {
          perr("option -p needs an argument. \n");
        }
        return 1;
    }
  }

  pthread_mutex_init(&mutex, NULL);

  NodeList* nodeList = InitNodeList();

  sock_id = initServer(portNo);

  while(1) {
    struct sockaddr_in caddr;
    socklen_t clen = sizeof(caddr);
    bzero(&caddr, sizeof(caddr));
    Args* args;
    char clientName[1024];
    args = (Args *)malloc(sizeof(Args));
    memset(args, 0, sizeof(args));
    sock_fd = accept(sock_id, (struct sockaddr *) &caddr, &clen);

    if( sock_fd == -1) {
      perr("Server accept error!");
    }

    getnameinfo(&caddr, clen, clientName, sizeof(clientName), NULL, 0, 0);
    /*printf("client name: %s\n", clientName);*/

    args->sock_fd = sock_fd;
    args->nodeList = nodeList;
    args->logFlg = logFlg;
    args->logPath = path;
    args->clientName = clientName;

    pthread_create(&threads[threadsCount], NULL, (void *) handleRequest, (void *) args);
    threadsCount++;
  }

  int i;
  for (i = 0; i < threadsCount; i++) {
    pthread_join(threads[i], NULL);
  }

  close(sock_fd);
  close(sock_id);
  return 0;
}

