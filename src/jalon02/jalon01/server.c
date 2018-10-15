#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <poll.h>
#include <time.h>

struct user{
    char* pseudo;
    char* ipAddress;
    int sockNumber;

};

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
        fflush(stdout);
        int j;
        int nbr_client = 0, max = 21, quit = 1;
        int *length=malloc(sizeof(int));
        *length = sizeof(&sockAddr);

        //On initialise la structure pollfd
        struct pollfd polls[max];
        memset(polls,0, sizeof(polls));
        polls[0].fd = sock;
        for(j=0 ; j<max; j++){
          polls[j].events = POLLIN;
        }

        //On initialise la structure user
        struct user users[max];
        memset(users,0, sizeof(user));

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
                if(polls[i].fd == sock){
                  //accept connection from client
                  int sock_client = do_accept(sock,(struct sockaddr *) &sockAddr,length);
                  if(sock_client != -1){
                    if(nbr_client < max){ //on vérifie si la liste d'attente est saturée

                      char *address = malloc(255);
                      char *pseudo = malloc(255);

                      int ipAddress = recv_message(sock_client,address,100);
                      users[nbr_client].ipAddress = ipAddress;
                      users[nbr_client].sock = sock_client;

                      int ok = send_message(sock_client,"/ok",255);
                      int user_pseudo = recv_message(sock_client,pseudo,255);
                      if(strncmp(pseudo,"/nick",5)==0){
                        char *user_pseudo = malloc(255);
                        user_pseudo = strchr(pseudo, ' ');
                        user_pseudo = user_pseudo+1;
                        users[nbr_client].pseudo = user_pseudo;
                      }

                      printf("pseudo client = %s\n",users[nbr_client].pseudo);
                      printf("adresse ip client = %s\n",users[nbr_client].ipAddress);
                      nbr_client +=1;
                      polls[nbr_client].fd = sock_client;
                      printf("nbr client = %d\n",nbr_client);

                    }
                    else{ //on prévient le client que sa requête ne peut pas être traitée pour le moment
                      int sockSend = send_message(sock_client,"trop de clients",255);
                    }
                  }
                }
              //On traite le cas où aucun nouveau client ne veut se connecter : on lit ce que ceux connectés ont à dire
              if(i != 0){
                //printf("essai\n");
                fflush(stdout);
                int sock_client = polls[i].fd;
                  char *buf = malloc(255);
                  //read what the client has to say
                  int nbBytes = recv_message(polls[i].fd,buf,255);
                  if(nbBytes > 0){
                    if(strncmp(buf,"/quit",5)==0){
                      printf("::Fermeture de la connexion avec le client %d\n", i);
                      close(polls[i].fd);
                      nbr_client = nbr_client - 1;
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

        //clean up client socket
    //clean up server socket
    close(sock);

    return 0;
}
