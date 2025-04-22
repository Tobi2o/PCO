/*  _____   _____ ____    ___   ___ ___  ____
 * |  __ \ / ____/ __ \  |__ \ / _ \__ \|___ \
 * | |__) | |   | |  | |    ) | | | | ) | __) |
 * |  ___/| |   | |  | |   / /| | | |/ / |__ <
 * | |    | |___| |__| |  / /_| |_| / /_ ___) |
 * |_|     \_____\____/  |____|\___/____|____/
 */
/**
 * @file cppmain.cpp
 * @brief En-tête pour le point d'entrée principal de la simulation de locomotives.
 * @date 2023-11-29
 * @author Christen Anthony, Haarun Ouweis
 *
 * Historique des modifications :
 * - Implémentation de la fonction d'arrêt d'urgence pour les locomotives.
 * - Ajout de la création et de la gestion des parcours de trains avec TrainTrack.
 * - Ajout de la gestion d'exceptions pour les erreurs de parcours.
 */


#include "ctrain_handler.h"
#include "locomotive.h"
#include "locomotivebehavior.h"
#include "synchrointerface.h"
#include "synchro.h"
#include "trainTrack.h"
#include <stdexcept>

// Locomotives :
// Vous pouvez changer les vitesses initiales, ou utiliser la fonction loco.fixerVitesse(vitesse);
// Laissez les numéros des locos à 0 et 1 pour ce laboratoire

// Locomotive A
static Locomotive locoA(7 /* Numéro (pour commande trains sur maquette réelle) */, 10 /* Vitesse */);
// Locomotive B
static Locomotive locoB(42 /* Numéro (pour commande trains sur maquette réelle) */, 12 /* Vitesse */);

// Arret d'urgence
void emergency_stop()
{
    locoA.arreter();
    locoA.fixerVitesse(0);
    locoB.arreter();
    locoB.fixerVitesse(0);

    afficher_message("\nSTOP!");
}


//Fonction principale
int cmain()
{
    /************
     * Maquette *
     ************/

    //Choix de la maquette (A ou B)
    selection_maquette(MAQUETTE_A /*MAQUETTE_B*/);

    /**********************************
     * Initialisation des aiguillages *
     **********************************/

    // Initialisation des aiguillages
    // Positiion de base donnée comme exemple, vous pouvez la changer comme bon vous semble
    // Vous devrez utiliser cette fonction pour la section partagée pour aiguiller les locos
    // sur le bon parcours (par exemple à la sortie de la section partagée) vous pouvez l'
    // appeler depuis vos thread des locos par ex.
    diriger_aiguillage(1,  TOUT_DROIT, 0);
    diriger_aiguillage(2,  DEVIE     , 0);
    diriger_aiguillage(3,  DEVIE     , 0);
    diriger_aiguillage(4,  TOUT_DROIT, 0);
    diriger_aiguillage(5,  TOUT_DROIT, 0);
    diriger_aiguillage(6,  TOUT_DROIT, 0);
    diriger_aiguillage(7,  TOUT_DROIT, 0);
    diriger_aiguillage(8,  DEVIE     , 0);
    diriger_aiguillage(9,  DEVIE     , 0);
    diriger_aiguillage(10, TOUT_DROIT, 0);
    diriger_aiguillage(11, TOUT_DROIT, 0);
    diriger_aiguillage(12, TOUT_DROIT, 0);
    diriger_aiguillage(13, TOUT_DROIT, 0);
    diriger_aiguillage(14, DEVIE     , 0);
    diriger_aiguillage(15, DEVIE     , 0);
    diriger_aiguillage(16, TOUT_DROIT, 0);
    diriger_aiguillage(17, TOUT_DROIT, 0);
    diriger_aiguillage(18, TOUT_DROIT, 0);
    diriger_aiguillage(19, TOUT_DROIT, 0);
    diriger_aiguillage(20, DEVIE     , 0);
    diriger_aiguillage(21, DEVIE     , 0);
    diriger_aiguillage(22, TOUT_DROIT, 0);
    diriger_aiguillage(23, TOUT_DROIT, 0);
    diriger_aiguillage(24, TOUT_DROIT, 0);
    // diriger_aiguillage(/*NUMERO*/, /*TOUT_DROIT | DEVIE*/, /*0*/);

    /********************************
     * Position de départ des locos *
     ********************************/

    // Loco 0
    // Exemple de position de départ
    locoA.fixerPosition(7, 14);

    // Loco 1
    // Exemple de position de départ
    locoB.fixerPosition(4, 10);

    /***********
     * Message *
     **********/

    // Affiche un message dans la console de l'application graphique
    afficher_message("Hit play to start the simulation...");

    /*********************
     * Threads des locos *
     ********************/

    // Création de la section partagée
    std::shared_ptr<SynchroInterface> sharedSectionSync = std::make_shared<Synchro>();

    // Création des circuits/parcours des locomotives avec gestion d'exceptions
    Railway trackA                = {7, 6, 5, 34, 33, 32, 25, 24, 23, 16, 15, 14};
    std::vector<Switch> switchesA = {{15, DEVIE}, {8, DEVIE}};
    TrainTrack* trainTrackA;

    Railway trackB                = {4, 3, 2, 1, 31, 30, 29, 28, 22, 24, 23, 16, 15, 10};
    std::vector<Switch> switchesB = {{16, DEVIE}, {15, TOUT_DROIT}, {8, TOUT_DROIT}, {7, DEVIE}};
    TrainTrack* trainTrackB;

    try {
        trainTrackA = new TrainTrack(&trackA, 5, 24, 15, switchesA);
        trainTrackB = new TrainTrack(&trackB, 2, 24, 15, switchesB);
    }  catch (const std::exception& e) {
        qDebug() << e.what();
        return -1;
    }

    // Création du thread pour la loco 0
    std::unique_ptr<Launchable> locoBehaveA = std::make_unique<LocomotiveBehavior>(locoA, sharedSectionSync, trainTrackA);
    // Création du thread pour la loco 1
    std::unique_ptr<Launchable> locoBehaveB = std::make_unique<LocomotiveBehavior>(locoB, sharedSectionSync, trainTrackB);

    // Lanchement des threads
    afficher_message(qPrintable(QString("Lancement thread loco A (numéro %1)").arg(locoA.numero())));
    locoBehaveA->startThread();
    afficher_message(qPrintable(QString("Lancement thread loco B (numéro %1)").arg(locoB.numero())));
    locoBehaveB->startThread();

    // Attente sur la fin des threads
    locoBehaveA->join();
    locoBehaveB->join();

    //Fin de la simulation
    mettre_maquette_hors_service();
    delete trainTrackA;
    delete trainTrackB;

    return EXIT_SUCCESS;
}
