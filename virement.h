#ifndef _VIREMENT_H_
#define _VIREMENT_H_

#define MAX_VIREMENT 100
#define NB_UTILISATEUR 1024

/* struct message used between server and client */
typedef struct
{
  int compteBeneficiaire;
  int compteEmetteur;
  int montant;

} Virement;

typedef struct
{
  Virement virements[MAX_VIREMENT];
  int tailleLogique;
  int type; // 1-Virement Normal
            // 2-Virement recurrent
} TabVirements;

typedef struct
{
  Virement virement;
  int statusEnvoie; // 0 - Pas d'envoie de virements
                    // 1 - Envoie des virements
} PipeComm;

typedef struct
{
  int numCompte;
  int montant;

} Compte;
#endif
