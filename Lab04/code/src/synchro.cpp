#include "synchro.h"

using namespace std;

Synchro::Synchro() : stationStop(0), stationMutex(1), trainWaitingAtStation(false),
                     sharedSectionStop(0), sharedSectionMutex(1), sharedSectionAvailable(true),
                     trainWaitingAtSharedSection(false), considerPriority(true) {}


void Synchro::access(Locomotive &loco) {
    afficher_message(qPrintable(QString("The engine no. %1 requests access to the shared section.").arg(loco.numero())));
    loco.afficherMessage("I would like to access the shared section.");

    sharedSectionMutex.acquire();
    while (!sharedSectionAvailable || (considerPriority and loco.priority < 0)) {
        trainWaitingAtSharedSection = true;
        sharedSectionMutex.release();

        loco.arreter();
        sharedSectionStop.acquire(); // Wait

        sharedSectionMutex.acquire();
        trainWaitingAtSharedSection = false;
        loco.priority = 0;
        loco.demarrer();
    }

    sharedSectionAvailable = false;
    sharedSectionMutex.release();

    loco.priority = 0;

    afficher_message(qPrintable(QString("The engine no. %1 accesses the shared section.").arg(loco.numero())));
    loco.afficherMessage("I access the shared section.");
}

void Synchro::leave(Locomotive& loco) {
    sharedSectionMutex.acquire();
    sharedSectionAvailable = true;
    considerPriority = false;

    if (trainWaitingAtSharedSection) {
        sharedSectionStop.release();
    }

    sharedSectionMutex.release();

    afficher_message(qPrintable(QString("The engine no. %1 leaves the shared section.").arg(loco.numero())));
    loco.afficherMessage("I leave the shared section.");
}

void Synchro::stopAtStation(Locomotive& loco) {
    afficher_message(qPrintable(QString("The engine no. %1 arrives at the station.").arg(loco.numero())));
    loco.afficherMessage("I arrive at the station.");

    loco.arreter();

    stationMutex.acquire();
    if (!trainWaitingAtStation) { // First train to arrive
        loco.afficherMessage("I'm the first one to arrive.");

        trainWaitingAtStation = true;
        stationMutex.release();

        stationStop.acquire(); // Wait on the other train

        loco.priority = -1;
    } else { // Second train to arrive
        loco.afficherMessage("I'm the second one to arrive.");

        PcoThread::thisThread()->usleep(5000000); // Wait 5 seconds

        trainWaitingAtStation = false;
        stationStop.release();
        stationMutex.release();

        sharedSectionMutex.acquire();
        considerPriority = true;
        sharedSectionMutex.release();

        loco.priority = 1;
    }


    loco.demarrer();

    if (loco.vitesse() > 0) {
        afficher_message(qPrintable(QString("The engine no. %1 leaves the station.").arg(loco.numero())));
        loco.afficherMessage("I leave the station.");
    }
}
