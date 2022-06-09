#include <stdio.h>
#include <stdlib.h>

#include "virement.h"
#include "utils_v1.h"

#define CLIENT_SERVEUR_SEM_KEY 5
#define CLIENT_SERVEUR_SHM_KEY 6
#define MAX_COMPTES 1000

int main(int argc, char const *argv[])
{
    Compte *comptes;
    if (argc != 3)
    {
        printf("Nombre d'arguments incorrect !\n");
        exit(1);
    }
    int numCompte = atoi(argv[1]);
    int montant = atoi(argv[2]);

    int shid = sshmget(CLIENT_SERVEUR_SHM_KEY, MAX_COMPTES * sizeof(Compte), 0);
    comptes = sshmat(shid);

    printf("Bienvenue dans le programme de depots et de retraits : \n");
    int sid = sem_get(CLIENT_SERVEUR_SEM_KEY, 1);

    sem_down0(sid);

    comptes[numCompte].montant += montant;
    printf("Nouveau solde:  %d \n", comptes[numCompte].montant);

    sem_up0(sid);
    sshmdt(comptes);

    return 0;
}
