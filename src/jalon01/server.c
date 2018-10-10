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
    printf("Nouvelle connexion établie : ");
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
  length = sockSend;
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
        int j;
        int nbr_client = 1, max = 21;
        int *length=malloc(sizeof(int));
        *length = sizeof(&sockAddr);
        //int sockAccept = do_accept(sock,(struct sockaddr *) &sockAddr,length);
        fflush(stdout);
        struct pollfd polls[max];
        memset(polls,0, sizeof(polls));
        polls[0].fd = sock;
        //polls[0].events = POLLIN;
        for(j=0 ; j<max; j++){
          polls[j].events = POLLIN;
        }
        int quit = 1;
        do {
          int i;
          int timeout = -1;

          int nbrpoll = poll(polls,max,timeout);

          //Erreur au niveau de la fonction poll
          if(nbrpoll < 0){
            perror("echec poll");
            fflush(stdout);
            break;
          }

          //Timeout écoulé ===> fermeture de la connexion
          else if (nbrpoll == 0){
            printf("Temps d'attente écoulé\n");
            fflush(stdout);
            break;
          }

          //Actions en attente
          else{
            for(i=0; i<nbr_client; i++){

              if(polls[i].revents == POLLIN){
                printf("%d\n",i);
                if(polls[i].fd == sock){
                  int sock_client = do_accept(sock,(struct sockaddr *) &sockAddr,length);
                  //printf("%d\n",sock_client);
                  if(sock_client != -1){
                    if(nbr_client < max){ //on vérifie si laliste d'attente est saturée
                      int msg = send_message(sock_client,"/ok",255);
                      printf("sock client %d\n",sock_client);
                      polls[nbr_client].fd = sock_client;

                      printf("nbr client = %d\n",nbr_client);

                      nbr_client +=1;
                    }
                    else{ //on prévient le client que sa requête ne peut pas être traitée pour le moment
                      int sockSend = send_message(sock_client,"trop de clients",255);
                    }
                  }
                }
              //On traite le cas où aucun nouveau client ne veut se connecter : on lit ce que ceux connectés ont à dire
              if(i != 0){
                printf("essai\n");
                fflush(stdout);
                int sock_client = polls[i].fd;

                  //printf("esssai\n");
                  char *buf = malloc(255);
                  //read what the client has to say
                  int nbBytes = recv_message(polls[i].fd,buf,255);
                  //printf("%d\n", nbBytes);
                  if(nbBytes > 0){
                    if(strncmp(buf,"/quit",5)==0){
                      printf("::Fermeture de la connexion\n");
                      quit = -1;
                      close(polls[i].fd);
                      nbr_client -= 1;
                      printf("nombre client = %d\n", nbr_client);
                    }
                    else{
                      printf(" [CLIENT %d ] : %s\n", i, buf);
                      //write back to the client
                      int sockSend = send_message(polls[i].fd,buf,255);
                    }
                  }
                  int msg = send_message(sock_client,"/ok",255);
                }
              }
            }
          }
        } while(quit != -1);



        //we write back to the client
        //clean up client socket
    //clean up server socket
    close(sock);

    return 0;
}
