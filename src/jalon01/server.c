#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

void sockaddr_in init_serv_addr(const char* port, struct sockaddr_in *serv_addr) {
    int portno;

//clean the serv_add structure
    memset(serv_addr, '0',sizeof(serv_addr));

//cast the port from a string to an int
    portno   = atoi(port);

//internet family protocol
    serv_addr->sin_family = AF_INET;

//we bind to any ip form the host
    serv_addr->sin_addr.s_addr = INADDR_ANY;

//we bind on the tcp port specified
    serv_addr->sin_port = htons(portno);

}

int do_socket(int domain, int type, int protocol) {
  int sockfd;
  int yes = 1;
  sockfd = socket(domain,type,protocol);
  if (sockfd == -1)
  {
    perror("socket");
    exit(EXIT_FAILURE);
  }
  else{
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1){
        error("ERROR setting socket options");
      }
    return sockfd;
  }
}

void do_bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen) {
  int sockBind;
  sockBind= bind(sockfd,addr,sizeof(*addr));
  if (sockBind == -1){
    perror("bind");
    exit(EXIT_FAILURE);
  }
}

int do_accept(int socket, struct sockaddr* addr, socklen_t* addrlen) {
  int sockAccept;
  sockAccept = accept(socket,addr,addrlen);
  if (sockAccept == -1) {
    perror("accept");
    exit(EXIT_FAILURE);
  }
  return sockAccept;
}

void do_read(int sockfd, char* buf, int len){
  int sockRead = 0;
  do{
    sockRead += read(sockfd, buf+sockRead, len-sockRead);
  } while(sockRead != len);
}

void do_write(int fd, const void *buf, size_t len){
  int sockWrite = 0;
  do{
    sockWrite += write(fd, buf+sockWrite, len-sockWrite);
  } while(sockWrite != len);
}

int main(int argc, char** argv)
{

    if (argc != 2)
    {
        fprintf(stderr, "usage: RE216_SERVER port\n");
        return 1;
    }


    //create the socket, check for validity!
    //do_socket()
    int sock = do_socket(PF_INET,SOCK_STREAM,0);


    //init the serv_add structure
    //init_serv_addr()
    struct sockaddr_in sockAddr;
    init_serv_addr(argv[1], sockAddr);

    //perform the binding
    //we bind on the tcp port specified
    //do_bind()
    do_bind(sock,sockAddr,sizeof(*sockAddr));

    //specify the socket to be a server socket and listen for at most 20 concurrent client
    //listen()
    int sockListen = listen(sock,20);
    if(sockListen== -1){
      perror("listen");
      exit(EXIT_FAILURE);
    }
    int i;
    for (i=0;i<21;i++)
    {

        //accept connection from client
        //do_accept()
        int sockAccept = do_accept(sock,sockAddr,sizeof(*sockAddr));

        while(true){
          char *buf = malloc(sizeof(char));
          int len;
          do{
            printf("Veuillez entrer la taille de votre message\n");
            scanf('%d',&len);
          } while(len < 0);
        //read what the client has to say
          do_read(sock, buf, len);
        }
        //we write back to the client
        do_write(sockAccept, buf, len);

        //clean up client socket
    }

    //clean up server socket
    close(sock);

    return 0;
}
