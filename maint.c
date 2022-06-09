#include <stdlib.h>
#include "utils_v1.h"
#include <unistd.h>
#include "virement.h"

#define CLIENT_SERVEUR_SEM_KEY 5
#define CLIENT_SERVEUR_SHM_KEY 6
#define PERM 0666
#define MAX_COMPTES 1000

int initialiserComptes(Compte *comptes)
{
    for (int i = 0; i < MAX_COMPTES; i++)
    {
        Compte compte;
        compte.montant = 0;
        compte.numCompte = i;

        comptes[i] = compte;
    }
}

int main(int argc, char const *argv[])
{
    int type = atoi(argv[1]);
    int opt;
    int sem_id;
    int shm_id;

    Compte *comptes;
    swrite(1, &type, sizeof(int));

    if (type == 1)
    {
        sem_create(CLIENT_SERVEUR_SEM_KEY, 1, PERM, 1);
        sem_id = sem_get(CLIENT_SERVEUR_SEM_KEY, 1);
        shm_id = sshmget(CLIENT_SERVEUR_SHM_KEY, MAX_COMPTES * sizeof(Compte), IPC_CREAT | PERM);
        comptes = sshmat(shm_id);

        sem_down0(sem_id);
        initialiserComptes(comptes);
        sem_up0(sem_id);

        sshmdt(comptes);
        printf("IPCs crée.\n");
    }
    else if (type == 2)
    {
        sem_id = sem_get(CLIENT_SERVEUR_SEM_KEY, 1);
        shm_id = shm_id = sshmget(CLIENT_SERVEUR_SHM_KEY, MAX_COMPTES * sizeof(Compte), IPC_CREAT | PERM);
        sshmdelete(shm_id);
        sem_delete(sem_id);
        printf("IPCs détruit.\n");
    }
    else if (type == 3)
    {
        if (argc != 3)
        {
            printf("Nombre d'arguments incorects !\n");
            exit(1);
        }
        sem_id = sem_get(CLIENT_SERVEUR_SEM_KEY, 1);
        opt = atoi(argv[2]);
        sem_down0(sem_id);
        printf("Memoire partager reservée.\n");
        sleep(opt);
        sem_up0(sem_id);
        printf("Memoire partager liberée.\n");
    }
    return 0;
}
