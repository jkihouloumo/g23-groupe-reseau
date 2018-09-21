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

void init_serv_addr(const char* port, struct sockaddr_in *serv_addr) {

    int portno;

//clean the serv_add structure
    memset(serv_addr, 0,sizeof(serv_addr));

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
  int b;
  b= bind(sockfd,addr,sizeof(*addr));
  if (b == -1){
    perror("bind");
    exit(EXIT_FAILURE);
  }
}

void do_accept(int socket, struct sockaddr* addr, socklen_t* addrlen) {
  int accepter;
  accepter = accept(socket,addr,addrlen);
  if (accepter == -1) {
    perror("accept");
    exit(EXIT_FAILURE);
  }
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
    int sock;
    sock = do_socket(PF_INET,SOCK_STREAM,0);


    //init the serv_add structure
    //init_serv_addr()
    init_serv_addr(argv[1],argv[0]);

    //perform the binding
    //we bind on the tcp port specified
    //do_bind()
    do_bind(sock,argv[0],sizeof(argv[0]));
    //specify the socket to be a server socket and listen for at most 20 concurrent client
    //listen()
    int ecoute;
    ecoute = listen(sock,20);
    if(ecoute == -1){
      perror("listen");
      exit(EXIT_FAILURE);
    }
    int i;
    for (i=0;i<21;i++)
    {

        //accept connection from client
        //do_accept()
        do_accept(sock,argv[0],sizeof(argv[0]));

        //read what the client has to say
        //do_read()

        //we write back to the client
        //do_write()

        //clean up client socket
    }

    //clean up server socket

    return 0;
}
