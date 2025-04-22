#include "trainTrack.h"
#include "ctrain_handler.h"
#include <algorithm>
#include <stdexcept>

using namespace std;

TrainTrack::TrainTrack(Railway* track, Contact station, Contact sharedSectionBegin, Contact sharedSectionEnd, std::vector<Switch> switches)
    : track(track), swicthes(switches), sharedSectionAccessOffset(3), sharedSectionLeaveOffset(1), switchesUpdateOffset(1) {

    auto stationIt            = std::find(track->begin(), track->end(), station);
    auto sharedSectionBeginIt = std::find(track->begin(), track->end(), sharedSectionBegin);
    auto sharedSectionEndIt   = std::find(track->begin(), track->end(), sharedSectionEnd);

    if (stationIt == track->end()) {
        throw new invalid_argument("Le contact d'entrée en gare spécifié ne se trouve pas sur le circuit fourni.");
    }

    if (sharedSectionBeginIt == track->end()) {
        throw new invalid_argument("Le contact de début de section partagée spécifié ne se trouve pas sur le circuit fourni.");
    }

    if (sharedSectionEndIt == track->end()) {
        throw new invalid_argument("Le contact de fin de section partagée spécifié ne se trouve pas sur le circuit fourni.");
    }

    if (sharedSectionEndIt <= sharedSectionBeginIt) {
        throw new invalid_argument("Le contact de fin de section partagée ne peut pas être égal ou se trouver avant celui de début.");
    }

    this->station              = &(*stationIt);
    this->sharedSection.first  = &(*sharedSectionBeginIt);
    this->sharedSection.second = &(*sharedSectionEndIt);
}

void TrainTrack::traveltToStation() {
    attendre_contact(*station);
}

void TrainTrack::travelToSharedSectionStart() {
    attendre_contact(*(sharedSection.first - sharedSectionAccessOffset));
}

void TrainTrack::travelToSharedSectionEnd() {
    attendre_contact(*(sharedSection.second + sharedSectionLeaveOffset));
}

void TrainTrack::updateSwicthes() {
    attendre_contact(*(sharedSection.first - switchesUpdateOffset));

    for (Switch s : swicthes) {
        diriger_aiguillage(s.first, s.second, 0);
    }
}
