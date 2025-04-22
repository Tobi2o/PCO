/*  _____   _____ ____    ___   ___ ___  ____
 * |  __ \ / ____/ __ \  |__ \ / _ \__ \|___ \
 * | |__) | |   | |  | |    ) | | | | ) | __) |
 * |  ___/| |   | |  | |   / /| | | |/ / |__ <
 * | |    | |___| |__| |  / /_| |_| / /_ ___) |
 * |_|     \_____\____/  |____|\___/____|____/
 */
/**
 * @file locomotivebehavior.h
 * @brief En-tête pour la classe LocomotiveBehavior, définissant le comportement d'une locomotive.
 * @date 2023-11-29
 * @author Christen Anthony, Harun Ouweis
 *
 * Historique des modifications :
 * - Ajout de l'attribut TrainTrack pour gérer le parcours de la locomotive.
 * - Modification du constructeur pour inclure le parcours de la locomotive.
 * - Implémentation de la logique de déplacement de la locomotive avec TrainTrack et SynchroInterface.
 */

#ifndef LOCOMOTIVEBEHAVIOR_H
#define LOCOMOTIVEBEHAVIOR_H

#include <pcosynchro/pcosemaphore.h>

#include "locomotive.h"
#include "launchable.h"
#include "synchrointerface.h"
#include "trainTrack.h"

/**
 * @brief La classe LocomotiveBehavior représente le comportement d'une locomotive
 */
class LocomotiveBehavior : public Launchable
{
public:
    /*!
     * \brief locomotiveBehavior Constructeur de la classe
     * \param loco la locomotive dont on représente le comportement
     */
    LocomotiveBehavior(Locomotive& loco, std::shared_ptr<SynchroInterface> sharedSectionSync, TrainTrack* trainTrack) :
        loco(loco),
        sharedSectionSync(sharedSectionSync),
        trainTrack(trainTrack)
    {
        // Eventuel code supplémentaire du constructeur
    }

protected:
    /*!
     * \brief run Fonction lancée par le thread, représente le comportement de la locomotive
     */
    void run() override;

    /*!
     * \brief printStartMessage Message affiché lors du démarrage du thread
     */
    void printStartMessage() override;

    /*!
     * \brief printCompletionMessage Message affiché lorsque le thread a terminé
     */
    void printCompletionMessage() override;

    /**
     * @brief loco La locomotive dont on représente le comportement
     */
    Locomotive& loco;

    /**
     * @brief sharedSection Pointeur sur la section partagée
     */
    std::shared_ptr<SynchroInterface> sharedSectionSync;

    /*
     * Vous êtes libres d'ajouter des méthodes ou attributs
     *
     * Par exemple la priorité ou le parcours
     */

    // Pointeur vers TrainTrack pour gérer le parcours de la locomotive
    TrainTrack* trainTrack;
};

#endif // LOCOMOTIVEBEHAVIOR_H
