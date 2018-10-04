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
<<<<<<< HEAD
  printf("::Prêt à lire, ");
  printf(" veuillez entrer un message à envoyer\n");
  fgets(str, maxlen, stdin);
=======
  printf("::Prêt à lire le message\n");
  printf("::Veuillez entrer un message à envoyer\n");
  fgets(str, maxlen, stdin);

>>>>>>> 7f64754f0d9c9541a52551316012532c9cfa2074
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
      //get user input
<<<<<<< HEAD

=======
      char *buf=malloc(255);
      char *str=malloc(255);
>>>>>>> 7f64754f0d9c9541a52551316012532c9cfa2074
      int length;
      do{
        printf(">>Veuillez entrer la taille du message\n");
        scanf("%d",&length);
      } while(length < 1);

      while(1){
<<<<<<< HEAD
        char *buf=malloc(255);
        readline(sockfd,buf,length);

        //send message to the server
        int sockSend = send_client_message(sockfd,buf,length);
        if(sockSend > 0){
          printf("::Message envoyé\n");
        }
        //handle_client_message(sockfd,str,length);
        if(strncmp(buf,"/quit",5)==0){
=======
        readline(sockfd,buf,length);

        //send message to the server
        printf("::Prêt à envoyer le message\n");
        int sockSend = send_client_message(sockfd,buf,length);
        //handle_client_message(sockfd,str,length);

        if(strcmp(buf,"/quit")==0){
>>>>>>> 7f64754f0d9c9541a52551316012532c9cfa2074
          printf("::Fermeture de la connexion\n");
          close(sockfd);
          break;
        }

<<<<<<< HEAD
        char *str=malloc(255);
        int sockRecv = recv_client_message(sockfd,str,255);
        printf(">> serveur dit : %s\n", str);
=======
        int sockRecv = recv_client_message(sockfd,str,length);
>>>>>>> 7f64754f0d9c9541a52551316012532c9cfa2074
      }
    return 0;
}
