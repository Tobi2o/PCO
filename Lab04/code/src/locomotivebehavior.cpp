/*  _____   _____ ____    ___   ___ ___  ____
 * |  __ \ / ____/ __ \  |__ \ / _ \__ \|___ \
 * | |__) | |   | |  | |    ) | | | | ) | __) |
 * |  ___/| |   | |  | |   / /| | | |/ / |__ <
 * | |    | |___| |__| |  / /_| |_| / /_ ___) |
 * |_|     \_____\____/  |____|\___/____|____/
 */

#include "locomotivebehavior.h"
#include "ctrain_handler.h"

void LocomotiveBehavior::run()
{
    //Initialisation de la locomotive
    loco.allumerPhares();
    loco.demarrer();
    loco.afficherMessage("Ready!");

    //Déplacement de la locomotive sur son circuit
    while(true) {
     /* La logique consiste à se rendre à la gare, accéder à la section partagée,
        mettre à jour les aiguillages, puis quitter la section partagée. */
        trainTrack->traveltToStation();
        sharedSectionSync->stopAtStation(loco);

        trainTrack->travelToSharedSectionStart();
        sharedSectionSync->access(loco);

        trainTrack->updateSwicthes();
        loco.afficherMessage("My switches have been updated.");

        trainTrack->travelToSharedSectionEnd();
        sharedSectionSync->leave(loco);
    }
}

void LocomotiveBehavior::printStartMessage()
{
    qDebug() << "[START] Thread de la loco" << loco.numero() << "lancé";
    loco.afficherMessage("Je suis lancée !");
}

void LocomotiveBehavior::printCompletionMessage()
{
    qDebug() << "[STOP] Thread de la loco" << loco.numero() << "a terminé correctement";
    loco.afficherMessage("J'ai terminé");
}
