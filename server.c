#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <poll.h>

#include "virement.h"
#include "server.h"
#include "utils_v1.h"

#define BACKLOG 5
#define CLIENT_SERVEUR_SEM_KEY 5
#define CLIENT_SERVEUR_SHM_KEY 6
#define PERM 0666
#define MAX_COMPTE 1000

int initSocketServer(int port)
{
  int sockfd = ssocket();

  sbind(port, sockfd);

  slisten(sockfd, BACKLOG);

  return sockfd;
}

int traitement(TabVirements tabVirement)
{
  int nouveauSoldeCompte;

  int sid = sem_get(CLIENT_SERVEUR_SEM_KEY, 1);

  int shid = sshmget(CLIENT_SERVEUR_SHM_KEY, MAX_COMPTE * sizeof(Compte), 0);

  Compte *comptes = sshmat(shid);

  sem_down0(sid);

  for (int i = 0; i < tabVirement.tailleLogique; i++)
  {
    int compteBenef = tabVirement.virements[i].compteBeneficiaire;
    int compteEmett = tabVirement.virements[i].compteEmetteur;
    int montant = tabVirement.virements[i].montant;

    comptes[compteEmett].montant -= montant;
    comptes[compteBenef].montant += montant;
    nouveauSoldeCompte = comptes[compteBenef].montant;
    printf("Virement : Compte du %d vers %d montant : %d \n", tabVirement.virements[i].compteEmetteur, tabVirement.virements[i].compteBeneficiaire, tabVirement.virements[i].montant);
  }

  sem_up0(sid);

  sshmdt(comptes);
  return nouveauSoldeCompte;
}

int main(int argc, char **argv)
{
  if (argc != 2)
  {
    perror("Erreur arguments manquants !\n");
    exit(1);
  }
  int serverPort, sockfd, newSockFd, nbSockFd;

  TabVirements tabVirements;

  struct pollfd fds[NB_UTILISATEUR];
  bool fds_invalid[NB_UTILISATEUR];

  nbSockFd = 0;
  serverPort = atoi(argv[1]);
  sockfd = initSocketServer(serverPort);

  printf("Le serveur tourne sur le port : %i \n", serverPort);

  fds[nbSockFd].fd = sockfd;
  fds[nbSockFd].events = POLLIN;
  nbSockFd++;

  while (true)
  {
    spoll(fds, nbSockFd, 0);

    if (fds[0].revents & POLLIN)
    {
      newSockFd = accept(sockfd, NULL, NULL);
      fds[nbSockFd].fd = newSockFd;
      fds[nbSockFd].events = POLLIN;
      nbSockFd++;
    }

    for (int i = 1; i < nbSockFd; i++)
    {

      if (fds[i].revents & POLLIN && !fds_invalid[i])
      {
        int nbRead = sread(fds[i].fd, &tabVirements, sizeof(tabVirements));
        if (nbRead != 0)
        {
          int nouveauSoldeCompte = traitement(tabVirements);
          if (tabVirements.type == 1)
          {
            swrite(fds[i].fd, &nouveauSoldeCompte, sizeof(nouveauSoldeCompte));
          }
        }

        if (tabVirements.type != 2 || nbRead <= 0)
        {
          sclose(fds[i].fd);
          fds_invalid[i] = true;
        }
      }
    }
  }
}
