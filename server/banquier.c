#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "banquier.h"

// Banquier stuff

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
  //verifie si la requete est legitime
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
  for (int i = 0; i < nbRes; i++)
  {
   
   available[i] = available[i] + allocation[i];
   *req = void;
  }
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

void
provision_with()