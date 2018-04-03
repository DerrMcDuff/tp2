/* This `define` tells unistd to define usleep and random.  */
#define _XOPEN_SOURCE 500

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

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


int main(int argc, char const *argv[])
{
  int a = random_ressources_request(10000,6);
  printf("%d\n", a);
  return 0;
}