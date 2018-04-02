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
extern Client_Process *client_processes;
extern Request *incoming_requests;
extern Request *unsatisfied_requests;
extern Request *queued_requests;

void add_request(Request **requests, int *ressources, int client);
void add_existing_request(Request **requests, Request *req);
void remove_first(Request **requests);
void print_requests(Request **requests);
int evaluate_request(Request* request);
void evaluate_incoming_requests();
void prepare_with(int clients_nb, int r_n);
Client_Process *get_process(int id);
