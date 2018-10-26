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
  int sock;
  int logged;
  char* date;
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
    printf("Nouvelle connexion établie... \n");
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

void display_message_server(char* msg){
  printf("[SERVEUR] : %s\n", msg);
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
  int sock_serveur = do_socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);

  //perform the binding
  //we bind on the tcp port specified
  //do_bind()time_t now = time (NULL);

  do_bind(sock_serveur,(struct sockaddr *) &sockAddr,sizeof(&sockAddr));

  //specify the socket to be a server socket and listen for at most 20 concurrent client
  //listen()
  int sockListen = listen(sock_serveur,20);
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
  int nbr_client = 1, max = 21, quit = 1, again = 0;;
  int *length=malloc(sizeof(int));
  *length = sizeof(&sockAddr);

  //On initialise la structure pollfd
  struct pollfd polls[max];
  memset(polls,0, sizeof(polls));
  polls[0].fd = sock_serveur;
  for(j=0 ; j<max; j++){
    polls[j].events = POLLIN;
  }

  //On initialise la structure user
  struct user users[max];
  memset(users,0, sizeof(users));
  for(j=0 ; j<max; j++){
    users[j].logged = 0;
  }

  do {
    int i;
    int timeout = -1;

    int nbrpoll = poll(polls,max,timeout);

    //Erreur au niveau de la fonction poll
    if(nbrpoll < 0){
      perror("echec poll");    int port;

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
          if(polls[i].fd == sock_serveur){
            //accept connection from client
            int sock_client = do_accept(sock_serveur,(struct sockaddr *) &sockAddr,length);
            //printf("%d\n", sock_client);
            if(sock_client != -1){
              if(nbr_client < max){ //on vérifie si la liste d'attente est saturée
              int initConnect = send_message(sock_client,"/init",255);
              char *address = malloc(255);
              char *pseudo = malloc(255);

              int ipAddress = recv_message(sock_client,address,100);
              users[nbr_client-1].ipAddress = address;
              users[nbr_client-1].sock = sock_client;




              int user_pseudo = recv_message(sock_client,pseudo,255);
              if(strncmp(pseudo,"/nick",5)==0){
                char *user_pseudo = malloc(255*sizeof(char));
                //const char s[2] = "\0";
                user_pseudo = strchr(pseudo, ' ');
                user_pseudo = user_pseudo+1;
                //user_pseudo = strtok(user_pseudo, s);
                users[nbr_client-1].pseudo = user_pseudo;
                users[nbr_client-1].logged = 1;
              }

              printf("%s est maintenant connecté\n",users[nbr_client-1].pseudo);
              //printf("ton adresse ip est %s\n",users[nbr_client-1].ipAddress);

              //if(users[nbr_client-1].logged == 1){
              //  int ok = send_message(sock_client,"/ok",255);
              //  }

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
        if(i != 0 ){
          //printf("essai\n");

          int sock_client = polls[i].fd;
          //if(users[i-1].logged == 1){
          //int msg = send_message(sock_client,"/ok",255);
          //  }
          char *buf=malloc(sizeof(char));
          memset(buf, '\0', 255);


          //read what the client has to say
          int nbBytes = recv_message(sock_client,buf,255);
          if(strncmp(buf, "\0",1) != 0){
            if(strncmp(buf,"/",1) == 0) {
              fflush(stdout);
              if(strncmp(buf,"/quit",5) == 0){

                printf("::Fermeture de la connexion avec %s\n", users[i-1].pseudo);
                close(polls[i].fd);
                users[nbr_client-1].logged = 0;

                nbr_client = nbr_client - 1;
                printf("nombre client = %d\n", nbr_client-1);
                fflush(stdout);
              }

              //Pour connaître tous les utilisateurs connectés
              if(strncmp(buf,"/who",4) == 0 && strncmp(buf,"/whoi",5) != 0){
                char *who = malloc(255*sizeof(char));
                int convert = sprintf(who,"%d",nbr_client-1);

                for(j=0; j<nbr_client-1; j++){
                  if(j==0){
                    int howMany = send_message(sock_client,who,255);
                  }
                  int user = send_message(sock_client, users[j].pseudo, 255);
                }
                fflush(stdout);
              }

              //Pour connaître les informations concernant un utilisateur
              if(strncmp(buf,"/whois ",7) == 0){

                char *pseudo1 = malloc(255*sizeof(char));
                char *WhoIs = malloc(255*sizeof(char));
                char *ip_client=malloc(255*sizeof(char));

                pseudo1 = strchr(buf, ' ');
                pseudo1 = pseudo1+1;

                fflush(stdout);
                int found=0;
                for(j=0;j<nbr_client-1;j++){
                  if(strncmp(users[j].pseudo,pseudo1,strlen(pseudo1)) == 0){
                    if(strlen(users[j].pseudo) == strlen(pseudo1)){
                      ip_client=users[j].ipAddress;
                      sprintf(WhoIs," %s est connecté à l'adresse Ip %s sur le port %s",pseudo1,ip_client,argv[1]);
                      int whois = send_message(sock_client,WhoIs,255);
                      printf("%s\n",WhoIs);
                      found=1;
                    }
                  }
                }

                if(found==0){
                  send_message(sock_client,"utilisateur introuvable\n",255);
                }
              }


              if(strncmp(buf,"/msg ",4) == 0 && strncmp(buf,"/msga ",5) != 0){
                char *pseudo1=malloc(255*sizeof(char));
                char *msg =malloc(255*sizeof(char));
                const char s[2] = " ";
                pseudo1 = strchr(buf, ' ');
                pseudo1 = pseudo1+1;
                msg=strchr(pseudo1,' ');
                msg=msg+1;
                pseudo1=strtok(pseudo1,s);
                fflush(stdout);
                int found=0;
                for(j=0;j<nbr_client-1;j++){
                  if(strncmp(users[j].pseudo,pseudo1,strlen(pseudo1)) == 0){
                    if(strlen(users[j].pseudo) == strlen(pseudo1)){
                      int client_sock=users[j].sock;
                      int sendmsg = send_message(client_sock,msg,255);
                      found=1;
                    }
                  }
                }
                if(found==0){
                  send_message(sock_client,"utilisateur introuvable\n",255);
                }
              }
            }

            else{
              printf(" [CLIENT %d ] : %s\n", i, buf);
              //write back to the client
              int sockSend = send_message(polls[i].fd,buf,255);
            }

          }



        }
      }
    }
  }

} while(quit != -1);

//clean up client socket
//clean up server socket
close(sock_serveur);

return 0;
}
