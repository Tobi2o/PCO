/*  _____   _____ ____    ___   ___ ___  ____
 * |  __ \ / ____/ __ \  |__ \ / _ \__ \|___ \
 * | |__) | |   | |  | |    ) | | | | ) | __) |
 * |  ___/| |   | |  | |   / /| | | |/ / |__ <
 * | |    | |___| |__| |  / /_| |_| / /_ ___) |
 * |_|     \_____\____/  |____|\___/____|____/
 */
/**
* @file synchro.h
* @brief En-tête pour la classe Synchro, implémente SynchroInterface pour la synchronisation des sections partagées.
* @date 2023-11-29
* @author Christen Anthony, Harun Ouweis
*
* Historique des modifications :
* - Ajout de sémaphores et de booléens pour la gestion de la station et de la section partagée.
* - Implémentation du constructeur Synchro.
* - Implémentation du constructeur Synchro avec initialisation des sémaphores.
* - Implémentation des méthodes pour la synchronisation des locomotives.
*/



#ifndef SYNCHRO_H
#define SYNCHRO_H

#include <QDebug>

#include <pcosynchro/pcosemaphore.h>
#include <pcosynchro/pcothread.h>

#include "locomotive.h"
#include "ctrain_handler.h"
#include "synchrointerface.h"

/**
 * @brief La classe Synchro implémente l'interface SynchroInterface qui
 * propose les méthodes liées à la section partagée.
 */
class Synchro final : public SynchroInterface
{
public:

    /**
     * @brief Synchro Constructeur de la classe qui représente la section partagée.
     * Initialisez vos éventuels attributs ici, sémaphores etc.
     */
    Synchro();

    /**
     * @brief access Méthode à appeler pour accéder à la section partagée
     *
     * Elle doit arrêter la locomotive et mettre son thread en attente si nécessaire.
     *
     * @param loco La locomotive qui essaie accéder à la section partagée
     */
    void access(Locomotive &loco) override;

    /**
     * @brief leave Méthode à appeler pour indiquer que la locomotive est sortie de la section partagée
     *
     * Réveille les threads des locomotives potentiellement en attente.
     *
     * @param loco La locomotive qui quitte la section partagée
     */
    void leave(Locomotive& loco) override;

    /**
     * @brief stopAtStation Méthode à appeler quand la locomotive doit attendre à la gare
     *
     * Implémentez toute la logique que vous avez besoin pour que les locomotives
     * s'attendent correctement.
     *
     * @param loco La locomotive qui doit attendre à la gare
     */
    void stopAtStation(Locomotive& loco) override;

private:
    // Ajout de sémaphores et booléens pour la gestion des sections partagées et des gares

    // Gares
    PcoSemaphore stationStop, stationMutex;
    bool trainWaitingAtStation;

    // Sections partagées
    PcoSemaphore sharedSectionStop, sharedSectionMutex;
    bool sharedSectionAvailable, trainWaitingAtSharedSection, considerPriority;
};


#endif // SYNCHRO_H
