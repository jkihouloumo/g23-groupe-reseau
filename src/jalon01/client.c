#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>



int main(int argc,char** argv)
{


    if (argc != 3)
    {
        fprintf(stderr,"usage: RE216_CLIENT hostname port\n");
        return 1;
    }
    struct addrinfo** res;
    //get address info from the server
    get_addr_info(argv[1], argv[2], res);

    //get the socket
    s = do_socket(hints.ai_family, hints.ai_socktype, hints.ai_protocol);

    //connect to remote socket
    do_connect()

    //get user input
    readline()

    //send message to the server
    handle_client_message()

    return 0;
}


void get_addr_info(const char* address, const char* port, struct addrinfo** res){
  int status;
  struct addrinfo hints;

  memset(&hints,0,sizeof(hints));

  hints.ai_family=AF_INET;
  hints.ai_socktype=SOCK_STREAM;
  hints.ai_protocol=0;
  hints.ai_flags=AI_PASSIVE;

  status = getaddrinfo(address,port,&hints,res);
  if (status == -1) {
    exit(EXIT_FAILURE);
  }
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
    msg += read(sockfd, buf+msg, len-msg;
  } while(msg!= len);
}


void handle_client_message(){
  int sockWrite = 0;
  do{
    sockWrite += write(fd, buf+sockWrite, len-sockWrite);
  } while(sockWrite != len);
}
