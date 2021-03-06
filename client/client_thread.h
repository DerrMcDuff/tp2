#ifndef CLIENTTHREAD_H
#define CLIENTTHREAD_H

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include <pthread.h>
#include <unistd.h>

/* Port TCP sur lequel le serveur attend des connections.  */
extern int port_number;

/* Nombre de requêtes que chaque client doit envoyer.  */
extern int num_request_per_client;

/* Nombre de resources différentes.  */
extern int num_resources;

/* Quantité disponible pour chaque resource.  */
extern int *provisioned_resources;

/* Quantite maximale pour chaque resource. */
extern int *max_resources;

typedef struct client_thread client_thread;
struct client_thread
{
  unsigned int id;
  pthread_t pt_tid;
  pthread_attr_t pt_attr;
};

int random_ressources_request (int max_res, int live_res);
void send_request (int client_id, int request_id, int socket_fd);
int ct_socket();
void ct_init (client_thread *);
void ct_create_and_start (client_thread *);
void ct_wait_server ();

void st_print_results (FILE *, bool);
void send_client_amount(int ressource_nb, int client_nb);

#endif // CLIENTTHREAD_H
