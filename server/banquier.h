#ifndef BANQUIER_H
#define BANQUIER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Processus/client
typedef struct Client_Process
{
		int id;
		int *alloc;
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

void add_request(Request **requests, int *ressources, int client);
void add_existing_request(Request **requests, Request *req);
void remove_first(Request **requests);
void print_requests(Request **requests);
void evaluate_incoming_requests();
int evaluate_request(Request* request);
void prepare_with(int clients_nb, int r_n);
Client_Process *get_process(int id);

#endif
