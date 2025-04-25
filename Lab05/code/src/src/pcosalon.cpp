/*  _____   _____ ____    ___   ___ ___  ____
 * |  __ \ / ____/ __ \  |__ \ / _ \__ \|___ \
 * | |__) | |   | |  | |    ) | | | | ) | __) |
 * |  ___/| |   | |  | |   / /| | | |/ / |__ <
 * | |    | |___| |__| |  / /_| |_| / /_ ___) |
 * |_|     \_____\____/  |____|\___/____|____/
 *
 * @file    pcosalon.cpp
 * @brief   Définition des méthodes de la classe PcoSalon déclarées dans le fichier d'en-tête 'pcosalon.h'.
 * @date    20.12.2023
 * @authors Harun Ouweis & Christen Anthony
 */
// Modifications à faire dans le fichier

#include "pcosalon.h"

#include <pcosynchro/pcothread.h>

#include <iostream>

using namespace std;

PcoSalon::PcoSalon(GraphicSalonInterface *interface, unsigned int capacity)
    : _interface(interface)
{
    this->_nbWaitingChairs         = capacity;
    this->_nbWorkingChairs         = 1;
    this->_capacity                = _nbWaitingChairs + _nbWorkingChairs;
    this->_nbWaitingChairsOccupied = 0;
    this->_barberSleeping          = false;
    this->_clientSupported         = false;
    this->_inService               = true;
}

/********************************************
 * Méthodes de l'interface pour les clients *
 *******************************************/
bool PcoSalon::accessSalon(unsigned clientId)
{    
    _mutex.lock();
    if (_clients.size() == _capacity) { // Le salon est plein
        _mutex.unlock();
        return false;
    }

    _clients.push_back(clientId); // Mise à jour de la liste des clients présents dans le salon

    _interface->consoleAppendTextClient(clientId, "J'entre dans le salon.");
     animationClientAccessEntrance(clientId);

     if (_barberSleeping) { // Le barbier dort
         _barberSleeping = false;
         _interface->consoleAppendTextClient(clientId, "Debout là-dedans !");
         _barberSleep.notifyOne(); // Le barbier se fait réveiller
     }

     if (_clients.front() != clientId) { // Le client doit attendre son tour
         unsigned waitingChairIndex = _nbWaitingChairsOccupied % _nbWaitingChairs;
         ++_nbWaitingChairsOccupied;

         _interface->consoleAppendTextClient(clientId, "Je m'assois et j'attends mon tour.");
         animationClientSitOnChair(clientId, waitingChairIndex);
     }
    _mutex.unlock();

    return true;
}


void PcoSalon::goForHairCut(unsigned clientId)
{
    _mutex.lock();
    while (_clients.front() != clientId) { // Le client doit attendre son tour
        _clientTurn.wait(&_mutex); // Mise en attente du client
    }

    _clientSupported = true;

    _interface->consoleAppendTextClient(clientId, "On s'occupe de moi.");
    animationClientSitOnWorkChair(clientId);

    _clientSeated.notifyOne(); // Le client est installé et le barbier peut commencer à l'embellir
    _beautify.wait(&_mutex);  // Début de l'embellissement
    _mutex.unlock();
}

void PcoSalon::waitingForHairToGrow(unsigned clientId)
{
    _mutex.lock();
    animationClientWaitForHairToGrow(clientId);
    _mutex.unlock();
}


void PcoSalon::walkAround(unsigned clientId)
{
    _mutex.lock();
    animationClientWalkAround(clientId);
    _mutex.unlock();
}


void PcoSalon::goHome(unsigned clientId)
{
    _mutex.lock();
    animationClientGoHome(clientId);
    _mutex.unlock();
}

/********************************************
 * Méthodes de l'interface pour le barbier  *
 *******************************************/
unsigned int PcoSalon::getNbClient()
{
    unsigned nbClients;

    _mutex.lock();
    nbClients = _clients.size();
    _mutex.unlock();

    return nbClients;
}

void PcoSalon::goToSleep()
{
    _mutex.lock();
    _barberSleeping = true;
    animationBarberGoToSleep();
    _barberSleep.wait(&_mutex); // Le barbier dort
    animationWakeUpBarber();
    _mutex.unlock();
}


void PcoSalon::pickNextClient()
{
    _mutex.lock();
    if (!_clientSupported) {
        _clientTurn.notifyAll(); // Au suivant !
    }
    _mutex.unlock();
}


void PcoSalon::waitClientAtChair()
{
    _mutex.lock();
    _clientSeated.wait(&_mutex); // Le barbier attend que le client s'installe pour commencer à l'embellir
    _mutex.unlock();
}


void PcoSalon::beautifyClient()
{
    _mutex.lock();
    animationBarberCuttingHair();
    _clientSupported = false;
    _clients.pop_front();
    _beautify.notifyOne(); // Fin de l'embellissement
    _mutex.unlock();
}

/********************************************
 *    Méthodes générales de l'interface     *
 *******************************************/
bool PcoSalon::isInService()
{
    bool inService;

    _mutex.lock();
    inService = _inService;
    _mutex.unlock();

    return inService;
}

void PcoSalon::endService()
{
    _mutex.lock();
    _inService = false;
    if (_barberSleeping) {
        _barberSleep.notifyOne(); // Le barbier est réveillé
    };
    _mutex.unlock();
}

/********************************************
 *   Méthodes privées pour les animations   *
 *******************************************/
void PcoSalon::animationClientAccessEntrance(unsigned clientId)
{
    _mutex.unlock();
    _interface->clientAccessEntrance(clientId);
    _mutex.lock();
}

void PcoSalon::animationClientSitOnChair(unsigned clientId, unsigned clientSitNb)
{
    _mutex.unlock();
    _interface->clientSitOnChair(clientId, clientSitNb);
    _mutex.lock();
}

void PcoSalon::animationClientSitOnWorkChair(unsigned clientId)
{
    _mutex.unlock();
    _interface->clientSitOnWorkChair(clientId);
    _mutex.lock();
}

void PcoSalon::animationClientWaitForHairToGrow(unsigned clientId)
{
    _mutex.unlock();
    _interface->clientWaitHairToGrow(clientId, true);
    _mutex.lock();
}

void PcoSalon::animationClientWalkAround(unsigned clientId)
{
    _mutex.unlock();
    _interface->clientWalkAround(clientId);
    _mutex.lock();
}

void PcoSalon::animationBarberGoToSleep()
{
    _mutex.unlock();
    _interface->barberGoToSleep();
    _mutex.lock();
}

void PcoSalon::animationWakeUpBarber()
{
    _mutex.unlock();
    _interface->clientWakeUpBarber();
    _mutex.lock();
}

void PcoSalon::animationBarberCuttingHair()
{
    _mutex.unlock();
    _interface->barberCuttingHair();
    _mutex.lock();
}

void PcoSalon::animationClientGoHome(unsigned clientId){
    _mutex.unlock();
    _interface->clientWaitHairToGrow(clientId, false);
    _mutex.lock();
}
