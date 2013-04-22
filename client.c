#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#define perr(msg) { perror(msg); exit(1); }

int main(int argc, char *argv[]) {
  static const char help[] =
    "\nDistributed Event Coordination System - Server:\n\n"
    "   -h              - Print this usage summary with all options.\n\n"
    "   -s server-host  - Connect to the specified host. The default server is localhost.\n\n"
    "   -p port-number  - Connect to the server at the given port. The default port is 9090.\n\n";

  int c;
  int sockfd;
  int len;
  int done;
  struct sockaddr_in address;
  int client;
  int bytes;
  int portNo = 9090;
  char line[256];
  char buffer[256];
  char *serverAddr = "127.0.0.1";
  bzero(line, 256);
  bzero(buffer, 256);
  FILE* file;
  fd_set ready;

  opterr = 0;

  while((c = getopt(argc, argv, "hs:p:")) != -1) {
    switch(c) {
      case 'h':
        printf("%s", help);
        exit(0);
        break;
      case 's':
        serverAddr = optarg;
        break;
      case 'p':
        portNo = atoi(optarg);
        if (portNo == 0 && optarg != '0') {
          perr("option -p argument type error! \n");
        }
        break;
      case '?':
        if (optopt == 's') {
          perr("option -s argument type error! \n");
        } else if (optopt == 'p') {
          perr("option -p argument type error! \n");
        }
        return 1;
    }
  }

  sockfd = socket(AF_INET, SOCK_STREAM, 0);
  address.sin_family = AF_INET;
  address.sin_addr.s_addr = inet_addr(serverAddr);
  address.sin_port = htons(portNo);
  len = sizeof(address);
  client = connect(sockfd, (struct sockaddr *)&address, len);

  if (client == -1) {
    perror("client init error!\n");
    exit(1);
  }
  file = stdin;
  while(!done) {
    /*fgets(line, 256, file);*/
    FD_ZERO(&ready);
    FD_SET(sockfd, &ready);
    FD_SET(fileno(stdin), &ready);
    if (select((sockfd + 1), &ready, 0, 0, 0) < 0) {
      perror("select");
      exit(1);
    }
    if (FD_ISSET(fileno(stdin), &ready)) {
      if((bytes = read(fileno(stdin), line, 256)) <= 0) {
        done++;
      }
      write(sockfd, line, 256);
    }
    if (FD_ISSET(sockfd, &ready)) {
      if ((bytes = read(sockfd, line, 256)) <= 0) {
        done++;
      }
      write(fileno(stdout), line, 256);
    }
    bzero(line, sizeof(line));
    /*write(sockfd, line, 256);*/
    /*if (read(sockfd, buffer, 256)) {*/
      /*printf("%s", buffer);*/
    /*} else {*/
      /*close(client);*/
    /*}*/
    /*bzero(buffer, sizeof(buffer));*/
  }
  close(client);
  exit(0);
}
