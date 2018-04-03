/* This `define` tells unistd to define usleep and random.  */
#define _XOPEN_SOURCE 500

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "client_thread.h"

// Socket library
#include <netdb.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>

int port_number = -1;
int num_request_per_client = -1;
int num_resources = -1;
int *provisioned_resources = NULL;
int *max_resources = NULL;
int *max_resources_per_client = NULL;
int socket_fd;

// Variable d'initialisation des threads clients.
unsigned int count = 0;

// Variable du journal.
// Nombre de requête acceptée (ACK reçus en réponse à REQ)
unsigned int count_accepted = 0;

// Nombre de requête en attente (WAIT reçus en réponse à REQ)
unsigned int count_on_wait = 0;

// Nombre de requête refusée (REFUSE reçus en réponse à REQ)
unsigned int count_invalid = 0;

// Nombre de client qui se sont terminés correctement (ACC reçu en réponse à END)
unsigned int count_dispatched = 0;

// Nombre total de requêtes envoyées.
unsigned int request_sent = 0;


int 
random_ressources_request (int max_res, int live_res)
{
  //on choisit une qte aleatoire de la meme ressource par requete
  //sans depasser le maximum pour le client
  //on choisit aleatoire si on demande ou si on libere une ressource
  int req;
  int r = rand()%(max_res);
  while((r+live_res > max_res) && (-r+live_res > max_res)){
   r = rand()%(max_res); 
  }
  int b = rand()%2;
  if(b==0) {
    req = r;
  }
  else 
  {
    req = -r;
  }

  return req;
}

//Fonction qui gere les reponses du serveur
void 
ct_server_response(char req[])
{
    char reponse[16]; // reponse du serveur
    FILE *socket_r = fdopen (socket_fd, "r");

    if(read(socket_fd, reponse ,255) < 0){
      perror("ERROR reading from socket");
    } 
    else {
      
      char *rep = strtok(reponse, " ");
      
      if(strcmp(rep, "ACK") == 0){
        count_accepted ++;
      }
      else if(strcmp(rep, "ERR") == 0){
        count_invalid ++;
      }
      else if(strcmp(rep, "WAIT") == 0){
        count_on_wait ++;
        int time = atoi(strtok(NULL, " "));

        //Tant que la requete n'a pas ete acceptee par le serveur
        //le client attend le temps que le serveur lui a dit d'attendre
        //et ensuite lui la renvoie
        while(strncmp(reponse, "ACK", 3) == 0){
          sleep(time);
          write(socket_fd, req, 64);
        }
      }
    }
    fflush(socket_r);
}

void
send_requests (int client_id, int socket_fd)
{
   
    char request[64] = "REQ ";
    sprintf(request, "%d ", socket_fd);
    
    //Boucle qui genere des requetes aleatoires par client
    for (int i = 0; i < num_request_per_client ; i++) {
      
      fprintf (stdout, "Client %d is sending its %d request\n", client_id, i);

      //Boucle qui genere les ressources demandees par requete
      for (int j = 0; j < num_resources; j++) {
        
        int req;
        int max_res = *(max_resources_per_client+j);
        int prov_res = *(provisioned_resources+j);
        
        //si c'est la derniere requete il faut s'assurer que toutes 
        //les ressources sont liberees
        if (i == num_request_per_client) {
          req = -(prov_res);
        }
        //sinon on alloue (ou libere) un nombre aleatoire de ressources
        else 
        {
          req = random_ressources_request(max_res, prov_res);
        }
        
        //rajouter la requete de cette ressource
        sprintf(request, "%d", socket_fd);
      }
      
      if(write(socket_fd, request, 64) < 0){
        perror("Send failed");
        exit(1);
      } else {
        
          usleep (random () % (100 * 1000));
          /* Attendre un petit peu (0s-0.1s) pour simuler le calcul.  */
          /* struct timespec delay;
            * delay.tv_nsec = random () % (100 * 1000000);
            * delay.tv_sec = 0;
            * nanosleep (&delay, NULL); */
          ct_server_response(request);
      }
      
      if(send(socket_fd, request, sizeof(request),0) < 0){
    }
    
    //Si toutes les requetes ont ete envoyees par le client_thread
    //Il annonce sa fermeture
    
    if (count_accepted == num_request_per_client){
      char request[64];
      strcpy(request, "CLO ");
      sprintf(request, "%d ", socket_fd);
      if(write(socket_fd, request, 64) < 0){
        perror("Send failed");
        exit(1);
      }


    }
  }
}

int
ct_socket()
{
  int client_socket = socket(AF_INET,SOCK_STREAM,0);
  
  if (client_socket < 0)
    perror ("ERROR opening socket");
    exit(1);
  
  struct sockaddr_in addr;
  addr.sin_family = AF_INET;
  addr.sin_port = htons(port_number); 
  addr.sin_addr.s_addr = htonl(0x7f000001);
  
  if (connect(client_socket, (struct sockaddr*)&addr, sizeof(addr))<0) {
    perror("ERROR on binding :(");
    exit(1);
  }
  return client_socket;
}

void 
send_max_resources(int res, int counter){
    *(max_resources + counter) = res;
}

void
send_client_amount(int ressource_nb, int client_nb)
{
  request_sent = request_sent + 1;
  int socket = ct_socket();
  FILE *socket_w = fdopen (socket, "w");
  
  char request[64] = "BEG ";
  sprintf(request, "%d ", num_resources);
  
  fflush(socket_w);
}

void
send_server_ressources(int *ressources)
{
  request_sent = request_sent + 1;
  int socket = ct_socket();
  FILE *socket_w = fdopen (socket, "w");
  
  char request[64] = "PRO ";
  for(int i=0; i<num_resources; i++){
    sprintf(request, "%d ", *(provisioned_resources+i));
  }
  sprintf(request, "%d ", "\n");
  
  
  fflush(socket_w);
}


//Fonction qui donne la valeur max des ressources d'un client-thread
void 
create_max_resources_for_client()
{
  max_resources_per_client = malloc (num_resources * sizeof (int));
  //Creation aleatoire des ressources maximales 
  //qu'un client_thread peux demander
  for (int j = 0; j < num_resources; j++) {
    *(max_resources_per_client + j) = rand()%(*max_resources + j);
    *(provisioned_resources + j) = 0;
  }
}

void *
ct_code (void *param)
{
  client_thread *ct = (client_thread *) param;
  socket_fd = ct_socket();
  FILE *socket_w = fdopen (socket_fd, "w");
  
  // TP2 TODO
  
  //Initialisation d'un client-thread 
  char clientIni[64];
  strcpy(clientIni, "INI ");
  sprintf(clientIni, "%d ", ct->id);
  create_max_resources_for_client();
  for(int i = 0; i < num_request_per_client; i++)
    sprintf(clientIni, "%d ", *(max_resources_per_client + i));
  sprintf(clientIni, "\n ");

  //envoie de la requete sur socket_w
  fputs(clientIni, socket_w);
  
  //Envoie des requetes de facon aleatoire 
  send_requests (ct->id, socket_fd);
  
  // fflsuh(socket_w);
  fclose(socket_w);
  return NULL;
}

//
// Vous devez changer le contenu de cette fonction afin de régler le
// problème de synchronisation de la terminaison.
// Le client doit attendre que le serveur termine le traitement de chacune
// de ses requêtes avant de terminer l'exécution.
//
void
ct_wait_server ()
{
  while((count_accepted + count_invalid) != num_request_per_client){
   sleep (4); 
  }
  
  count_dispatched ++;
  char finish[16];
  strcpy(finish, "END");
  write(socket_fd, finish, 64);

}


void
ct_init (client_thread * ct)
{
  ct->id = count++;
}

void
ct_create_and_start (client_thread * ct)
{
  pthread_attr_init (&(ct->pt_attr));
  pthread_create (&(ct->pt_tid), &(ct->pt_attr), &ct_code, ct);
  pthread_detach (ct->pt_tid);
}

//
// Affiche les données recueillies lors de l'exécution du serveur.
// La branche else ne doit PAS être modifiée.
//
void
st_print_results (FILE * fd, bool verbose)
{
  if (fd == NULL)
    fd = stdout;
  if (verbose)
  {
    fprintf (fd, "\n---- Résultat du client ----\n");
    fprintf (fd, "Requêtes acceptées: %d\n", count_accepted);
    fprintf (fd, "Requêtes : %d\n", count_on_wait);
    fprintf (fd, "Requêtes invalides: %d\n", count_invalid);
    fprintf (fd, "Clients : %d\n", count_dispatched);
    fprintf (fd, "Requêtes envoyées: %d\n", request_sent);
  }
  else
  {
    fprintf (fd, "%d %d %d %d %d\n", count_accepted, count_on_wait,
        count_invalid, count_dispatched, request_sent);
  }
}
