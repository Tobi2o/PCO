/*  _____   _____ ____    ___   ___ ___  ____
 * |  __ \ / ____/ __ \  |__ \ / _ \__ \|___ \
 * | |__) | |   | |  | |    ) | | | | ) | __) |
 * |  ___/| |   | |  | |   / /| | | |/ / |__ <
 * | |    | |___| |__| |  / /_| |_| / /_ ___) |
 * |_|     \_____\____/  |____|\___/____|____/
 *
 * @file    client.cpp
 * @brief   Définition des méthodes de la classe Client déclarées dans le fichier d'en-tête 'client.h'.
 * @date    20.12.2023
 * @authors Harun Ouweis & Christen Anthony
 */
// Modifications à faire dans le fichier

#include "client.h"
#include <unistd.h>

#include <iostream>

int Client::_nextId = 0;

Client::Client(GraphicSalonInterface *interface, std::shared_ptr<SalonClientInterface> salon)

    : _interface(interface), _salon(salon),  _clientId(_nextId++)
{
    _interface->consoleAppendTextClient(_clientId, "Salut, prêt pour une coupe !");
}

void Client::run()
{
    while (_salon->isInService()) {
        // Tentative d'accéder au salon
        _interface->consoleAppendTextClient(_clientId, "Je tente d'accéder au salon.");
        if (_salon->accessSalon(_clientId)) { // Le salon n'est pas plein

            _salon->goForHairCut(_clientId);
            _interface->consoleAppendTextClient(_clientId, "Quelle coupe, merci ! A la prochaine.");

            // Attente pour la repousse des cheveux
            _salon->waitingForHairToGrow(_clientId);
        } else { // Le salon est plein
            _interface->consoleAppendTextClient(_clientId, "Le salon est plein. Je vais faire un tour.");
            _salon->walkAround(_clientId);
        }
    }
    // Le client rentre chez lui une fois le service terminé
    _salon->goHome(_clientId);
    _interface->consoleAppendTextClient(_clientId, "Le salon est fermé... Zut !");
}
