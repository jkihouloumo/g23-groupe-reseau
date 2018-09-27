#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>


int do_socket(int domain, int type, int protocol) {
  int sockfd;
  int yes = 1;
  sockfd = socket(domain,type,protocol);
  if (sockfd == -1)
  {
    perror("socket");
    exit(EXIT_FAILURE);


  }
  else
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1){
        error("ERROR setting socket options");
      }
    return sockfd;
  }



void do_connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen) {

    int res = connect(sockfd, addr, addrlen);

    if (res != 0) {
      perror("connect");
      exit(EXIT_FAILURE);
    }

}

void readline(int fd, void *str, size_t maxlen){
  int msg = 0;
  printf("Veuillez entrer un message à envoyer\n");
  fgets(str, maxlen, stdin);
  do{
    msg += read(fd, str+msg, maxlen-msg);
  } while(msg!= maxlen);
}


void handle_client_message(int fd, const void *str, size_t maxlen){
  int sockWrite = 0;
  do{
    sockWrite += write(fd, str+sockWrite, maxlen-sockWrite);
  } while(sockWrite != maxlen);
}

int main(int argc,char** argv)
{


    if (argc != 3)
    {
        fprintf(stderr,"usage: RE216_CLIENT hostname port\n");
        return 1;
    }

    printf("En attente d'un serveur\n");
    
    struct sockaddr_in sock_host;
    int sockfd;
    int port = atoi(argv[2]);
    //get the socket
    sockfd = do_socket(AF_INET,SOCK_STREAM,0);

    memset(&sock_host, '\0', sizeof(sock_host));
    sock_host.sin_family = AF_INET;
    sock_host.sin_port = htons(port);
    inet_aton(argv[1], &sock_host.sin_addr);
    //get address info from the server


    //connect to remote socket
    do_connect(sockfd,(struct sockaddr *) &sock_host,sizeof(sock_host));

    //get user input
    char *str=malloc(10);

    readline(sockfd,str,100);

    //send message to the server
    handle_client_message(sockfd,str,100);

    return 0;
}
