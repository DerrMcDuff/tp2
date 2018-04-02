/* This `define` tells unistd to define usleep and random.  */
#define _XOPEN_SOURCE 500

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
//#include <time.h>

#include "client_thread.h"

// Socket library
//#include <netdb.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

int port_number = -1;
int num_request_per_client = -1;
int num_resources = -1;
int *provisioned_resources = NULL;
int *max_resources = NULL;
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

//time_t t;
//srand((unsigned)time(&t));

// Vous devez modifier cette fonction pour faire l'envoie des requêtes
// Les ressources demandées par la requête doivent être choisies aléatoirement
// (sans dépasser le maximum pour le client). Elles peuvent être positives
// ou négatives.
// Assurez-vous que la dernière requête d'un client libère toute les ressources
// qu'il a jusqu'alors accumulées.

int 
random_ressources_request (int max_res, int live_res)
{
  //on choisit une qte aleatoire de la meme ressource par requete
  //sans depasser le maximum pour le client
  //on choisit aleatoire si on demande ou si on libere une ressource
  int req;
  int r = rand()%(max_res);
  int b = rand()%2;
  if(b==0) req = r;
  else req = -r;

  return req;
}


void
send_request (int client_id, int request_id, int socket_fd)
{
  
  char request[64];
  strcpy(request, "REQ ");
  sprintf(request, "%d ", socket_fd);
  
  fprintf (stdout, "Client %d is sending its %d request\n", client_id,
      request_id);

  for (int i = 0; i < num_request_per_client ; i++) {
    for (int j = 0; j < num_resources; j++) {
      
      int req;
      int max_res = *(max_resources+j);
      int prov_res = *(provisioned_resources+j);
      
      //si c'est la derniere requete il faut s'assurer que toutes 
      //les ressources sont liberees
      if(i == num_request_per_client) {
        req = -(*(provisioned_resources+j));
      }
      
      //sinon on alloue (ou libere) un nombre aleatoire de ressources
      else {
        req = random_ressources_request(max_res, prov_res);
      }
      
      //rajouter la requete de cette ressource
      sprintf(request, "%d ", socket_fd);
    }
    
    
    if(send(socket_fd, request, 64, 0) < 0){
      perror("Send failed");
      exit(1);
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

void *
ct_code (void *param)
{
  // Initialisation d'un socket
  int socket_fd = ct_socket();
  client_thread *ct = (client_thread *) param;
  FILE *socket_w = fdopen (socket_fd, "w");
  
  char *request = NULL;

  printf("Enter the request:\n");
  if(fgets(request, 256, stdin) == 0){
    
      fprintf(stderr, "BRUH\n");
    //envoie la requete au serveur
    fputs(request, socket_w);
    
    //initialisation des petits clients ('INI') et de leurs ressources
    
    if(strncmp(strtok(request, " "),"INI", 3) == 0){
      
        ct->id = * strtok(NULL, " ");
        
        while(strtok(NULL, " ") != NULL){
            //mettre le pointeur de max resources sur la qte max de la 1e ressource
            //mettre le pointeur des ressources allouees sur la valeur 0
            //incrementer le pointeur pour les prochaines resources
            *max_resources = atoi(strtok(NULL, " "));
            *provisioned_resources = 0;
            max_resources++;
            provisioned_resources++;
        }
        
    } else if(strncmp(strtok(NULL, " "),"CLO", 3) == 0){
      
    }
  
  }else {
    perror("Request not valid.");
    exit(1);
  }


  for (unsigned int request_id = 0; request_id < num_request_per_client;
      request_id++)
  {

    // TP2 TODO
    // Vous devez ici coder, conjointement avec le corps de send request,
    // le protocole d'envoi de requête.

    send_request (ct->id, request_id, socket_fd);

    // TP2 TODO:END

    /* Attendre un petit peu (0s-0.1s) pour simuler le calcul.  */
    usleep (random () % (100 * 1000));
    /* struct timespec delay;
     * delay.tv_nsec = random () % (100 * 1000000);
     * delay.tv_sec = 0;
     * nanosleep (&delay, NULL); */
  }
  
  fflush(socket_w);

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

  // TP2 TODO: IMPORTANT code non valide.
  
//  for (int i = 0; i++; i < num_request_per_client) {
//    if (...){
//      count_dispatched += 1;
//    }
//    else{
//        sleep (4);
//    }
//  }
  sleep(4);
  // TP2 TODO:END
  
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
// Affiche les données recueillies lors de l'exécution du
// serveur.
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

void
send_client_amount(int ressource_nb, int client_nb) {
  int socket = ct_socket();
  FILE *socket_w = fdopen (socket, "w");
  fprintf (socket_w, "BEG %d %d\n",ressource_nb,client_nb);
}
