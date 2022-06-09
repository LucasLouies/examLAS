#ifndef _CLIENT_H_
#define _CLIENT_H_

/**
 * Crée un objet de type Virement
 *PRE: ServierIP : une adresse IP valide
 *     ServerPort : un numéro de port valide
 *POST: en cas de succès, connecte un socket client à ServerIP:port
        return le descripteur de fichier de socket
        en cas d'échec, affiche la cause de l'erreur et quitte le programme
 */
int initSocketClient(char *ServerIP, int Serverport);

/**
 * Crée un objet de type Virement
 * PRE: cp_emett : compte émetteur.
 *      cp_benef : compte bénéficiaire.
 *      montant : montant du virement.
 * RES: Un objet Virement est renvoyé avec les infos reçues en paramètre
 */
Virement creationVirement(int cp_emett, int cp_benef, int montant);

/**
 * PRE: argv : pipe de virement valide
 * POST: génère un battement de cœur à intervalle régulier et d’un programme “fils” qui gère une série
 *       de virements récurrents émis à chaque battement de cœur
 */
void minuterie(void *argv);

/**
 *
 * PRE: virement : le virement de type Virement
 * POST: Effectue un virement simple
 */
void effectuerVirementSimple(Virement virement);

/**
 *
 * PRE: argv : pipe de virement valide
 * POST: Enregistre un virement récurrent dans le pipe
 */
void effectuerVirementRecurrent(void *argv);

void terminal();

#endif