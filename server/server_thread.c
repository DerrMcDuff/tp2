#define _XOPEN_SOURCE 700   /* So as to allow use of `fdopen` and `getline`.  */

#include "server_thread.h"

#include <netinet/in.h>
#include <netdb.h>

#include <fcntl.h>
#include <strings.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/socket.h>

#include <signal.h>

#include <time.h>

// Homemade
#include "banquier.h"

enum { NUL = '\0' };

enum {
  /* Configuration constants.  */
  max_wait_time = 30,
  server_backlog_size = 5
};

int server_socket_fd;

// Nombre de client enregistré.
int nb_registered_clients;

// Variable du journal.
// Nombre de requêtes acceptées immédiatement (ACK envoyé en réponse à REQ).
unsigned int count_accepted = 0;

// Nombre de requêtes acceptées après un délai (ACK après REQ, mais retardé).
unsigned int count_wait = 0;

// Nombre de requête erronées (ERR envoyé en réponse à REQ).
unsigned int count_invalid = 0;

// Nombre de clients qui se sont terminés correctement
// (ACK envoyé en réponse à CLO).
unsigned int count_dispatched = 0;

// Nombre total de requête (REQ) traités.
unsigned int request_processed = 0;

// Nombre de clients ayant envoyé le message CLO.
unsigned int clients_ended = 0;

// TODO: Ajouter vos structures de données partagées, ici.

static void sigint_handler(int signum) {
  // Code terminaison.
  accepting_connections = 0;
}

void
st_init ()
{
  // Handle interrupt
  signal(SIGINT, &sigint_handler);


  // TODO

}

void
st_process_requests (server_thread * st, int socket_fd)
{
  // TODO: Remplacer le contenu de cette fonction
  FILE *socket_r = fdopen (socket_fd, "r");
  FILE *socket_w = fdopen (socket_fd, "w");

  while (true)
  {
    char cmd[4] = {NUL, NUL, NUL, NUL};
    if (!fread (cmd, 3, 1, socket_r))
      break;
    char *args = NULL; size_t args_len = 0;
    ssize_t cnt = getline (&args, &args_len, socket_r);
    if (!args || cnt < 1 || args[cnt - 1] != '\n')
    {
      printf ("Thread %d received incomplete cmd=%s!\n", st->id, cmd);
      break;
    }

    printf ("Thread %d received the command: %s%s", st->id, cmd, args);

    if (strcmp(cmd,"INI") == 0) {
      
    } else if (strcmp(cmd,"REQ") == 0) {

    } else if (strcmp(cmd,"CLO") == 0) {
      
    } else if (strcmp(cmd,"BEG") == 0) {

      nb_registered_clients = atoi (&args[3]);
      int nb_ressourceeee = atoi (&args[2]);
      prepare_with(nb_registered_clients, nb_ressourceeee);
    }

    fprintf (socket_w, "ERR Unknown command\n");
    free (args);
  }

  fclose (socket_r);
  fclose (socket_w);
  // TODO end
}

int st_wait() {
  struct sockaddr_in thread_addr;
  socklen_t socket_len = sizeof (thread_addr);
  int thread_socket_fd = -1;
  int end_time = time (NULL) + max_wait_time;

  while(thread_socket_fd < 0 && accepting_connections) {
    thread_socket_fd = accept(server_socket_fd,
        (struct sockaddr *)&thread_addr,
        &socket_len);
    if (time(NULL) >= end_time) {
      break;
    }
  }
  return thread_socket_fd;
}

void *
st_code (void *param)
{
  server_thread *st = (server_thread *) param;
  
  int thread_socket_fd = -1;

  // Boucle de traitement des requêtes.
  while (accepting_connections)
  {
    // Wait for a I/O socket.
    thread_socket_fd = st_wait();
    if (thread_socket_fd < 0)
    {
      fprintf (stderr, "Time out on thread %d.\n", st->id);
      continue;
    }

    if (thread_socket_fd > 0)
    {
      st_process_requests (st, thread_socket_fd);
      close (thread_socket_fd);
    }
  }
  return NULL;
}


//
// Ouvre un socket pour le serveur.
//
void
st_open_socket (int port_number)
{
  server_socket_fd = socket (AF_INET, SOCK_STREAM, 0);
  if (server_socket_fd < 0) {
    perror ("ERROR opening socket");
    exit(1);
  }
    int sockopt = fcntl(server_socket_fd, F_GETFL);
    fcntl(server_socket_fd, F_SETFL, sockopt | O_NONBLOCK);

  if (setsockopt(server_socket_fd, SOL_SOCKET, SO_REUSEADDR, &(int){ 1 }, sizeof(int)) < 0) {
    perror("setsockopt()");
    exit(1);
  }

  struct sockaddr_in serv_addr;
  memset (&serv_addr, 0, sizeof (serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr = INADDR_ANY;
  serv_addr.sin_port = htons (port_number);

  if (bind
      (server_socket_fd, (struct sockaddr *) &serv_addr,
       sizeof (serv_addr)) < 0)
    perror ("ERROR on binding");

  listen (server_socket_fd, server_backlog_size);
}


//
// Affiche les données recueillies lors de l'exécution du
// serveur.
// La branche else ne doit PAS être modifiée.
//
void
st_print_results (FILE * fd, bool verbose)
{
  if (fd == NULL) fd = stdout;
  if (verbose)
  {
    fprintf (fd, "\n---- Résultat du serveur ----\n");
    fprintf (fd, "Requêtes acceptées: %d\n", count_accepted);
    fprintf (fd, "Requêtes : %d\n", count_wait);
    fprintf (fd, "Requêtes invalides: %d\n", count_invalid);
    fprintf (fd, "Clients : %d\n", count_dispatched);
    fprintf (fd, "Requêtes traitées: %d\n", request_processed);
  }
  else
  {
    fprintf (fd, "%d %d %d %d %d\n", count_accepted, count_wait,
        count_invalid, count_dispatched, request_processed);
  }
}

void add_request(Request **requests, Client_Process *client)
{
  if (*requests == NULL)
  {
    *requests = malloc(sizeof(Request));
    (*requests)->demandeur = client;
    (*requests)->next = NULL;
  }
  else
  {
    Request *current = (*requests);
    while (current->next != NULL) 
    {
          current = current->next;
      }
      current->next = malloc(sizeof(Request));
      current->next->demandeur = client;
      current->next->next = NULL;
  }
}

void remove_first(Request **requests) 
{
  Request *old = *requests;
  *requests = (*requests)->next;
  free(old);
}

void print_requests(Request **requests) 
{
  printf("%d\n",(*requests)->demandeur->id);
  Request *current = (*requests);
  while (current->next != NULL) 
  {
    printf("%d\n",current->next->demandeur->id);
        current = current->next;
    }
}


//Fonction qui determine si une requete peux etre effectuee
//Si oui, elle est mise dans la file dattente
//Sinon, elle reste dans la file des requetes
void 
bankers_algo(Request *req, int nbRes, int nbClient)
{

  int need[nbRes];
  //verifie si la requete est legit
  // if(client_processes[id] == req.demandeur.id)
  
  for (int i = 0; i < nbRes; i++)
  {
    need[i] = req->demandeur->max[i] - req->demandeur->alloc[i];
    if (need[i] > available[i])
    { // la requete ne peux etre satisfaite pour l'instant :(
      exit(1);
    }
  }
  
  //Si la requete est acceptee elle est mise dans la file d'exec
  // for (int i = 0; i < nbRes; i++)
  // {
  //  // satisfiedQueue(req);
  //  available[i] = available[i] + allocation[i];
  //  *req = void;
  // }
}

void
prepare_with(int clients_nb, int r_n)
{
  ressource_nb = r_n;
  client_processes = malloc (clients_nb * sizeof (struct Client_Process));
  // for (int i = 0; i < clients_nb; i++)
  // {
  //  client_processes[i] = malloc (ressource_nb * sizeof (int));
  // }
}











