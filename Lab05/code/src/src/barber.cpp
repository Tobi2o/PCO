/*  _____   _____ ____    ___   ___ ___  ____
 * |  __ \ / ____/ __ \  |__ \ / _ \__ \|___ \
 * | |__) | |   | |  | |    ) | | | | ) | __) |
 * |  ___/| |   | |  | |   / /| | | |/ / |__ <
 * | |    | |___| |__| |  / /_| |_| / /_ ___) |
 * |_|     \_____\____/  |____|\___/____|____/
 *
 * @file    barber.cpp
 * @brief   Définition des méthodes de la classe Barber déclarées dans le fichier d'en-tête 'barber.h'.
 * @date    20.12.2023
 * @authors Harun Ouweis & Christen Anthony
 */
// Modifications à faire dans le fichier

#include "barber.h"
#include <unistd.h>

#include <iostream>

Barber::Barber(GraphicSalonInterface *interface, std::shared_ptr<SalonBarberInterface> salon)
    : _interface(interface), _salon(salon)
{
    _interface->consoleAppendTextBarber("Salut, prêt à travailler !");
}

void Barber::run()
{
    while (_salon->isInService() or _salon->getNbClient() > 0) {
        if (_salon->getNbClient() == 0) { // Si aucun client n'attend, le barbier dort
            _interface->consoleAppendTextBarber("Pas de clients ! Je vais faire une sieste.");
            _salon->goToSleep();
            continue;
        }

        // Le barbier prend le prochain client
        _interface->consoleAppendTextBarber("Au suivant !");
        _salon->pickNextClient();

        // Le barbier attend que le client s'assoie
        _interface->consoleAppendTextBarber("[Attente]");
        _salon->waitClientAtChair();

        // Le barbier coiffe le client
        _interface->consoleAppendTextBarber("Coupe, coupe, coupe, ...");
        _salon->beautifyClient();
        _interface->consoleAppendTextBarber("C'est terminé !");
    }
    _interface->consoleAppendTextBarber("La journée est terminée, à demain !");
}
