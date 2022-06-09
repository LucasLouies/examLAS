#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "virement.h"
#include "utils_v1.h"
#include "client.h"

#define MAX_CHAR_COMMANDE 50

volatile sig_atomic_t status = 0;

int compteEmetteur;
char *serverIp;
int port;
int delay;

void handlerStatus(int num)
{
  status = 1;
}

int initSocketClient(char *ServerIP, int Serverport)
{
  int sockfd = ssocket();
  sconnect(ServerIP, Serverport, sockfd);
  return sockfd;
}

Virement creationVirement(int cp_emett, int cp_benef, int montant)
{
  Virement virement;
  virement.compteBeneficiaire = cp_benef;
  virement.compteEmetteur = cp_emett;
  virement.montant = montant;

  return virement;
}

void minuterie(void *argv)
{
  int *pipe = argv;

  PipeComm pipeComm;
  pipeComm.statusEnvoie = 1;
  sclose(pipe[0]);

  while (status == 0)
  {
    swrite(pipe[1], &pipeComm, sizeof(pipeComm));
    sleep(delay);
  }
  sclose(pipe[1]);
}

void effectuerVirementSimple(Virement virement)
{
  printf("j'effectue le virement ! Montant : %d, Compte: %d\n", virement.montant, virement.compteBeneficiaire);

  int nouveauSoldeCompte;

  TabVirements tabVirements;
  tabVirements.virements[0] = virement;
  tabVirements.tailleLogique = 1;
  tabVirements.type = 1;

  int sockFd = initSocketClient(serverIp, port);
  swrite(sockFd, &tabVirements, sizeof(tabVirements));

  sread(sockFd, &nouveauSoldeCompte, sizeof(nouveauSoldeCompte));
  printf("Nouveau solde du compte numero %d : %d\n", virement.compteBeneficiaire, nouveauSoldeCompte);
  sclose(sockFd);
}

void effectuerVirementRecurrent(void *argv)
{

  int *pipeVirement = argv;

  int sockFd = -1;

  sclose(pipeVirement[1]);

  PipeComm pipeComm;

  TabVirements tabVirements;
  tabVirements.tailleLogique = 0;
  tabVirements.type = 2;

  int nbReadChar = sread(pipeVirement[0], &pipeComm, sizeof(pipeComm));
  while (nbReadChar > 0)
  {

    if (pipeComm.statusEnvoie == 1)
    {

      sockFd = initSocketClient(serverIp, port);
      swrite(sockFd, &tabVirements, sizeof(tabVirements));
      sclose(sockFd);
    }
    else
    {
      tabVirements.virements[tabVirements.tailleLogique] = pipeComm.virement;
      tabVirements.tailleLogique++;
    }
    nbReadChar = sread(pipeVirement[0], &pipeComm, sizeof(pipeComm));
  }
  sclose(pipeVirement[0]);
}

void terminal()
{

  int nbCharRead = 0;
  char command[MAX_CHAR_COMMANDE];
  char *msg;
  int status = 0; // Status : 0 - Fils virementRecurrent n'existe pas encore
                  //        1 - Fils virementRecurrent existe déja
  int pid_minuterie;

  printf("Bienvenue dans la banque :\n");
  printf("Pour éffectué un virement normal tapper la commande : '+ [numero_compte_beneficiaire] [montant]'\n");
  printf("Pour éffectué un virement recurrent tapper la commande : '* [numero_compte_beneficiaire] [montant]'\n");
  printf("Pour quitter le programme tapper q \n");

  ssigaction(SIGUSR1, handlerStatus);

  do
  {
    write(1, ">", 1);
    nbCharRead = sread(STDIN_FILENO, command, MAX_CHAR_COMMANDE);

    int cp_benef, montant;
    char *symbole = strtok(command, " ");
    char *cp_benef_str = strtok(NULL, " ");
    char *montant_str = strtok(NULL, " ");

    if (cp_benef_str != NULL || montant_str != NULL)
    {
      cp_benef = atoi(cp_benef_str);
      montant = atoi(montant_str);
    }

    int pipeVirement[2];

    Virement virement = creationVirement(compteEmetteur, cp_benef, montant);

    switch (*symbole)
    {
    case '+':
      effectuerVirementSimple(virement);
      break;
    case '*':

      if (status == 0)
      {

        spipe(pipeVirement);

        fork_and_run1(effectuerVirementRecurrent, pipeVirement);

        pid_minuterie = fork_and_run1(minuterie, pipeVirement);

        sclose(pipeVirement[0]);

        status = 1;
      }
      PipeComm pipeComm;
      pipeComm.virement = virement;
      pipeComm.statusEnvoie = 0;
      swrite(pipeVirement[1], &pipeComm, sizeof(pipeComm));
      break;
    case 'q':

      if (pid_minuterie != 0)
      {
        skill(pid_minuterie, SIGUSR1);
      }
      sclose(pipeVirement[1]);
      printf("Fin du programme.\n");
      exit(0);

    default:
      msg = "Commande invalide ! Veuillez recommencer : \n";
      swrite(1, msg, strlen(msg));
      continue;
    }
  } while (nbCharRead > 0);
}

int main(int argc, char **argv)
{
  if (argc != 5)
  {
    perror("Nombre d'arguments incorrect !\n");
    exit(EXIT_FAILURE);
  }

  serverIp = argv[1];
  port = atoi(argv[2]);
  compteEmetteur = atoi(argv[3]);
  delay = atoi(argv[4]);

  terminal();

  return 0;
}
