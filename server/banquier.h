#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Processus/client
typedef struct Client_Process
{
		int id;
		int *alloc ;
		int *max;
		
} Client_Process;

// Requete
typedef struct Request
{
	int *req;
	Client_Process *demandeur;
	struct Request *next;

} Request;

// Listes qui seront utilisées
int ressource_nb;
int *available;
Client_Process *client_processes;
Request *incoming_requests;
Request *unsatisfied_requests;
Request *queued_requests;

void add_request(Request **requests, Client_Process *client);
void remove_first(Request **requests);
void print_requests(Request **requests);
void bankers_algo(Request *req, int nbRes, int nbClient);
void prepare_with(int clients_nb, int r_n);
