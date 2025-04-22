/**
 * @file traintrack.h
 * @brief En-tête pour la classe TrainTrack, responsable de la gestion des circuits de train.
 * @date 2023-11-29
 * @author Christen Anthony, Harun Ouweis
 *
 * Historique des modifications :
 * - Création de la classe TrainTrack pour gérer les circuits, y compris les contacts, sections partagées et aiguillages.
 * - Ajout de la vérification des paramètres et de la gestion des exceptions.
 */

#ifndef TRAINTRACK_H
#define TRAINTRACK_H

#include <vector>
#include <utility>

using Contact = unsigned;
using Railway = std::vector<Contact>;
using Section = std::pair<Contact*, Contact*>;
using Switch  = std::pair<unsigned, unsigned>;

class TrainTrack {
public:

    /**
     * @brief Constructeur de la classe TrainTrack
     *
     * @param track Pointeur vers le vecteur représentant le parcours du train.
     * @param station Contact représentant la gare.
     * @param sharedSectionBegin Contact de début de la section partagée.
     * @param sharedSectionEnd Contact de fin de la section partagée.
     * @param switches Vecteur de paires représentant les aiguillages et leur état.
     * @throws std::invalid_argument Si un des contacts fournis n'est pas valide.
     */
    TrainTrack(Railway* track, Contact station, Contact sharedSectionBegin, Contact sharedSectionEnd, std::vector<Switch> switches);

    // Méthodes de navigation sur la voie ferrée
    /**
     * @brief Déplace le train jusqu'à la gare.
     */
    void traveltToStation();

    /**
     * @brief Déplace le train jusqu'au début de la section partagée.
     */
    void travelToSharedSectionStart();

    /**
     * @brief Déplace le train jusqu'à la fin de la section partagée.
     */
    void travelToSharedSectionEnd();

    /**
     * @brief Met à jour les aiguillages sur le parcours du train.
     */
    void updateSwicthes();

private:
    Railway*            track;
    Contact*            station;
    Section             sharedSection;
    std::vector<Switch> swicthes;
    unsigned            sharedSectionAccessOffset;
    unsigned            sharedSectionLeaveOffset;
    unsigned            switchesUpdateOffset;
};

#endif // TRAINTRACK_H
