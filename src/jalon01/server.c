#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <poll.h>
void error(const char *msg)
{
    perror(msg);
    exit(1);
}

void init_serv_addr(const char* port, struct sockaddr_in *serv_addr) {
    int portno;

//clean the serv_add structure
    memset(serv_addr, '\0' ,sizeof(serv_addr));

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
  else{
    printf("Connexion établie \n");
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

int send_message(int sock,char *buffer,int length){
  int sockSend = send(sock,buffer,length,0);
  if (sockSend==-1){
    perror("erreur d'envoi");
  }
  return sockSend;
}

int recv_message(int sock,char *buffer,int length){
  int sockRecv = recv(sock,buffer,length,0);
  if (sockRecv==-1){
    perror("erreur de reception");
  }
  return sockRecv;
}
int main(int argc, char** argv)
{

    if (argc != 2)
    {
        fprintf(stderr, "usage: RE216_SERVER port\n");
        return 1;
    }

    printf("Initialisation du serveur ...\n");
    //init the serv_add structure
    //init_serv_addr()
    struct sockaddr_in sockAddr;
    init_serv_addr(argv[1], &sockAddr);

    //create the socket, check for validity!
    //do_socket()
    int sock = do_socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);

    //perform the binding
    //we bind on the tcp port specified
    //do_bind()

    do_bind(sock,(struct sockaddr *) &sockAddr,sizeof(&sockAddr));

    //specify the socket to be a server socket and listen for at most 20 concurrent client
    //listen()
    int sockListen = listen(sock,20);
    if(sockListen== -1){
      perror("listen");
      exit(EXIT_FAILURE);
    }
    else{
      int port = atoi(argv[1]);
      printf("::Serveur en écoute sur le port %d\n", port);
    }
        //accept connection from client
        //do_accept()
        int nbr_client = 0;
        int *length=malloc(sizeof(int));
        *length = sizeof(&sockAddr);
<<<<<<< HEAD
        //int sockAccept = do_accept(sock,(struct sockaddr *) &sockAddr,length);
        fflush(stdout);
        struct pollfd polls[25];
        polls[0].fd = sock;
        polls[0].events = POLLIN;

        //polls[1].fd = sockAccept;
=======
        int sockAccept = do_accept(sock,(struct sockaddr *) &sockAddr,length);
        fflush(stdout);
        while(1){
          char *buf = malloc(sizeof(char));
          char *str = malloc(sizeof(char));
          int nbBytes = recv_message(sockAccept,buf,255);
          if(strcmp(buf,"/quit")==0){
            printf("::Fermeture de la connexion\n");
            close(sock);
            break;
          }
          else{
            printf("<< %s\n", buf);
          }
          int sockSend = send_message(sockAccept,buf,255);
          //read what the client has to say
          //do_read(sock, buf, len);
          //printf("%s", buf);
          //do_write(sockAccept, buf, len);

>>>>>>> 7f64754f0d9c9541a52551316012532c9cfa2074



        while(1){
          int i;
          int timeout = -1;

          int nbrpoll = poll(polls,nbr_client,timeout);

          while(nbrpoll>0){
            printf("oeoeoe");

            if (polls[0].revents != 0){
              if(nbr_client < 21){
                nbr_client +=1;
                polls[nbr_client].fd = do_accept(sock,(struct sockaddr *) &sockAddr,length);
                printf("nbr client est égal à %d\n",nbr_client);

                char *buf = malloc(sizeof(char));
                //read what the client has to say
                //do_read(sock, buf, len);
                int nbBytes = recv_message(polls[nbr_client].fd,buf,255);
                if(nbBytes == 0) {
                  printf("pas de message\n");
                  break;
                }

                if(strncmp(buf,"/quit",5)==0){
                  printf("::Fermeture de la connexion\n");
                  close(sock);
                  break;
                }
                else{
                  printf(">> client numéro %d dit : %s\n", nbr_client, buf);
                }
                //do_write(sockAccept, buf, len);
                int sockSend = send_message(polls[nbr_client].fd,buf,255);
              }
              else{
                int sockSend = send_message(polls[nbr_client].fd,"0",255);
              }
            }
          }
        }



        //we write back to the client
        //clean up client socket
<<<<<<< HEAD
=======


>>>>>>> 7f64754f0d9c9541a52551316012532c9cfa2074
    //clean up server socket
    close(sock);

    return 0;
}
