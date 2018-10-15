#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>


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
        perror("ERROR setting socket options");
      }
    return sockfd;
  }



int do_connect(int sockfd, const struct sockaddr *addr, int addrlen) {

    int res = connect(sockfd, addr, addrlen);

    if (res != 0) {
      printf("::Erreur de connexion\n");
      exit(EXIT_FAILURE);
    }
    return res;
}

void readline(int fd, void *str, size_t maxlen){
  int msg = 0;
  printf("::Prêt à lire \n");
  fgets(str, maxlen, stdin);
}


void handle_client_message(int fd, const void *str, size_t maxlen){
  int sockWrite = 0;
  do{
    sockWrite += write(fd, str+sockWrite, maxlen-sockWrite);
  } while(sockWrite != maxlen);
}

int send_client_message(int sock,char *buffer,int length){
  int sockSend = send(sock,buffer,length,0);
  if (sockSend==-1){
    perror("erreur d'envoi");
  }
  length = sockSend;
  return sockSend;
}

int recv_client_message(int sock,char *buffer,int length){
  int sockRecv = recv(sock,buffer,length,0);
  if (sockRecv==-1){
    perror("erreur de reception");
  }
  return sockRecv;
}

int quit_chat(char* buf){
  int quit = 1;
  if(strncmp(buf,"/quit",5)==0){
    printf("::Fermeture de la connexion\n");
    quit = -1;
  }
  return quit;
}

void display_message_server(char* msg){
  printf("[SERVEUR] : %s\n", msg);
}

int main(int argc,char** argv)
{


    if (argc != 3)
    {
        fprintf(stderr,"usage: RE216_CLIENT hostname port\n");
        return 1;
    }

    printf("En attente d'un serveur ...\n");

    struct sockaddr_in sock_host;
    int sockfd;
    int port = atoi(argv[2]);
    //get the socket
    sockfd = do_socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);

    memset(&sock_host, '\0', sizeof(sock_host));
    sock_host.sin_family = AF_INET;
    sock_host.sin_port = htons(port);
    inet_aton("argv[1]", &sock_host.sin_addr);
    //get address info from the server

    //connect to remote socket
    int sockConnect = do_connect(sockfd,(struct sockaddr *) &sock_host,sizeof(sock_host));
    if(sockConnect != -1){
      printf("::Connecté au serveur sur le port %d\n", port);
    }
      fflush(stdout);

      char *fromServer=malloc(255*sizeof(char));
      char *init2=malloc(255*sizeof(char));
      char *buf=malloc(255*sizeof(char));
      char *nbrClient=malloc(255*sizeof(char));
      char *str=malloc(255*sizeof(char));
      int length=255;
      int quit = 1;
      int init = 0;


      do{
        //get user input
        int recvInit = recv_client_message(sockfd,fromServer,255);
        printf("[SERVEUR] : %s\n", fromServer);
        //display_message_server(fromServer);

        //Attends le feu vert du serveur pour envoyer son pseudo
        if(strncmp(fromServer, "/init",5)==0){
          init = 1;
          send_client_message(sockfd, argv[1], 100); //envoi de l'adresse ip du client

          do{ //on s'assure que le client rentre son pseudo comme il faut
            printf("[SERVEUR] : please logon with /nick <your pseudo>\n");
            readline(sockfd,buf,length);
            quit = quit_chat(buf);
            if(quit == -1){
              int sendQuit = send_client_message(sockfd,str,length);
              break;
            }

          } while(strncmp(buf, "/nick ", 6) != 0);
          char *pseudo = malloc(255*sizeof(char));
          pseudo = strchr(buf, ' ');
          printf("[SERVEUR] Bienveue sur le chat%s\n", pseudo);

          //Envoi du pseudo au serveur
          int username = send_client_message(sockfd,buf,length);

        }

        if(strncmp(fromServer, "/ok",3)==0 && init == 1){
        //Attends le feu vert du serveur pour envoyer un message
          //printf("nouvelle boucle\n");
          readline(sockfd,str,length);
          quit = quit_chat(str);
          if(quit == -1){
            int sendQuit = send_client_message(sockfd,str,length);
            break;
          }
          /*if(strncmp(str,"/who",4)){
            int recvNbClient = recv_client_message(sockfd, nbrClient,255);
            int nbr = atoi(nbrClient);
            printf("[SERVEUR] Il y a %d profil(s) connecté(s ) :\n", nbr);
          }*/
          int sockSend = send_client_message(sockfd,str,length);
        }

      } while(quit != -1);


    return 0;
}
