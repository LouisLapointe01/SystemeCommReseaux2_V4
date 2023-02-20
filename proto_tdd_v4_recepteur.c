/*************************************************************
* proto_tdd_v0 -  récepteur                                  *
* TRANSFERT DE DONNEES  v4                                   *
*                                                            *
* Protocole sans contrôle de flux, sans reprise sur erreurs  *
*                                                            *
* Univ. de Toulouse III - Paul Sabatier         *
**************************************************************/

#include <stdio.h>
#include "application.h"
#include "couche_transport.h"
#include "services_reseau.h"

/* =============================== */
/* Programme principal - récepteur */
/* =============================== */
int main(int argc, char* argv[])
{

    paquet_t paquet, ACKpaquet;
    paquet_t buffer[SEQ_NUM_SIZE];

    int fin = 0; /* condition d'arrêt */
    int borne_inf=0;
    int verification[SEQ_NUM_SIZE] = {0}; 
    

    init_reseau(RECEPTION);

    printf("[TRP] Initialisation reseau : OK.\n");
    printf("[TRP] Debut execution protocole transport.\n");

    /* tant que le récepteur reçoit des données */
    while ( !fin ) {

        //attendre(); /* optionnel ici car de_reseau() fct bloquante */
        de_reseau(&paquet);

        if (verifier_controle(paquet) == 1)
        {
            
            if (!verification[paquet.num_seq] && dans_fenetre(borne_inf, paquet.num_seq, SEQ_NUM_SIZE/2))
            {
                verification[paquet.num_seq] = 1;
                buffer[paquet.num_seq] = paquet;
            }

            while (verification[borne_inf])
            {

                /* remise des données à la couche application */
                fin = vers_application(buffer[borne_inf].info, buffer[borne_inf].lg_info);

                printf("fin= %d\n",fin);
                verification[borne_inf] = 0;

                borne_inf = (borne_inf+1)%SEQ_NUM_SIZE;

            }
            
            ACKpaquet.num_seq = paquet.num_seq;
            ACKpaquet.type = ACK;           ACKpaquet.lg_info = 0;
            ACKpaquet.somme_ctrl = generer_controle(ACKpaquet);
            
            vers_reseau(&ACKpaquet);
        }
    }
    printf("[TRP] Fin execution protocole transport.\n");
    return 0;
}
