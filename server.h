#ifndef _CLIENT_H_
#define _CLIENT_H_

/**
 * PRE:  ServerPort : un numéro de port valide
 * POST: en cas de succès, lies un socket à 0.0.0.0:port et l'ecoute
       return le descripteur de fichier de socket
       en cas d'échec, affiche la cause de l'erreur et quitte le programme
 */
int initSocketServer(int port);
/**
 *
 * PRE: tabVirements :  Tableau de virements
 * POST: Modifies les données dans la memoire partagée
 * RES: Nouveau solde du compte émetteur.
 */
int traitement(TabVirements tabVirements);

#endif