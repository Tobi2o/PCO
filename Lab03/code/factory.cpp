/**
 * @file factory.cpp
 * @brief En-tête pour la classe Factory avec des modifications pour la synchronisation et la gestion de ressources.
 * - Se  situe normalement danns le .h mais exceptionnellement ici pour ce laboratoire.
 * @date 2023-11-08
 * @author Christen Anthony, Harun Ouweis
 *
 * Historique des modifications :
 * - Ajout de la gestion de la concurrence dans les méthodes `buildItem`, `orderResources`, `run` et `trade`
 *   pour protéger les ressources partagées en utilisant un mutex.
 * - Intégration d'une condition d'arrêt propre dans la méthode `run` pour permettre une terminaison gracieuse du thread.
 * - Implémentation de la logique de commande de ressources dans `orderResources` pour gérer les stocks insuffisants.
 * - Mise à jour de la méthode `trade` pour effectuer des transactions thread-safe de l'objet construit.
 */

#include "factory.h"
#include "extractor.h"
#include "costs.h"
#include "wholesale.h"
#include <pcosynchro/pcothread.h>
#include <iostream>

WindowInterface* Factory::interface = nullptr;


Factory::Factory(int uniqueId, int fund, ItemType builtItem, std::vector<ItemType> resourcesNeeded)
    : Seller(fund, uniqueId), resourcesNeeded(resourcesNeeded), itemBuilt(builtItem), nbBuild(0)
{
    assert(builtItem == ItemType::Chip ||
           builtItem == ItemType::Plastic ||
           builtItem == ItemType::Robot);

    interface->updateFund(uniqueId, fund);
    interface->consoleAppendText(uniqueId, "Factory created");
}

void Factory::setWholesalers(std::vector<Wholesale *> wholesalers) {
    Factory::wholesalers = wholesalers;

    for(Seller* seller: wholesalers){
        interface->setLink(uniqueId, seller->getUniqueId());
    }
}

ItemType Factory::getItemBuilt() {
    return itemBuilt;
}

int Factory::getMaterialCost() {
    return getCostPerUnit(itemBuilt);
}

bool Factory::verifyResources() {
    for (auto item : resourcesNeeded) {
        if (stocks[item] == 0) {
            return false;
        }
    }

    return true;
}


void Factory::buildItem() {
    int employeeCost = getEmployeeSalary(getEmployeeThatProduces(itemBuilt));

    resourcesProtector.lock(); // Début S.C.
    if (money < employeeCost) {
        resourcesProtector.unlock(); // Fin S.C.
        /* Pas assez d'argent */
        return;
    }
    /* On peut payer un employé */
    money -= employeeCost;
    ++nbBuild;

    for (auto resourceUsed : resourcesNeeded) {
        --stocks[resourceUsed];
    }
    resourcesProtector.unlock(); // Fin S.C.

    //Temps simulant l'assemblage d'un objet.
    PcoThread::usleep((rand() % 100) * 100000);

    resourcesProtector.lock(); // Début S.C.
    ++stocks[getItemBuilt()];
    resourcesProtector.unlock(); // Fin S.C.

    interface->consoleAppendText(uniqueId, "Factory have build a new object");
}

void Factory::orderResources() {
    int qty = 1;

    for (auto resource : resourcesNeeded) {
        if (stocks[resource] == 0) {
            for (auto wholesaler : wholesalers) {

                resourcesProtector.lock(); // Début S.C.
                if (money < getCostPerUnit(resource) * qty) {
                    resourcesProtector.unlock(); // Fin S.C.
                    continue;
                }
                resourcesProtector.unlock(); // Fin S.C.

                int bill = wholesaler->trade(resource, qty);

                if (bill == 0) {
                    continue;
                }

                resourcesProtector.lock(); // Début S.C.
                money -= bill;
                ++stocks[resource];
                resourcesProtector.unlock(); // Fin S.C.

                interface->consoleAppendText(uniqueId, QString("I bought %1 ").arg(qty) % getItemName(resource) % QString(" wich costed me %1").arg(bill));
                break;
            }
        }
    }

    //Temps de pause pour éviter trop de demande
    PcoThread::usleep(10 * 100000);
}

void Factory::run() {
    if (wholesalers.empty()) {
        std::cerr << "You have to give to factories wholesalers to sales their resources" << std::endl;
        return;
    }
    interface->consoleAppendText(uniqueId, "[START] Factory routine");

    while (!PcoThread::thisThread()->stopRequested()) {
        if (verifyResources()) {
            buildItem();
        } else {
            orderResources();
        }
        interface->updateFund(uniqueId, money);
        interface->updateStock(uniqueId, &stocks);
    }
    interface->consoleAppendText(uniqueId, "[STOP] Factory routine");
}

std::map<ItemType, int> Factory::getItemsForSale() {
    return std::map<ItemType, int>({{itemBuilt, stocks[itemBuilt]}});
}

int Factory::trade(ItemType it, int qty) {
    if (qty <= 0 || it != getItemBuilt() || stocks[it] < qty) {
        return 0;
    }

    resourcesProtector.lock(); // Début S.C.
    if (stocks[it] < qty) {
        resourcesProtector.unlock(); // Fin S.C.
        return 0;
    }
    stocks[it] -= qty;
    money += getMaterialCost() * qty;
    resourcesProtector.unlock(); // Fin S.C.

    interface->updateFund(uniqueId, money);
    interface->updateStock(uniqueId, &stocks);

    return getMaterialCost() * qty;
}

int Factory::getAmountPaidToWorkers() {
    return Factory::nbBuild * getEmployeeSalary(getEmployeeThatProduces(itemBuilt));
}

void Factory::setInterface(WindowInterface *windowInterface) {
    interface = windowInterface;
}

PlasticFactory::PlasticFactory(int uniqueId, int fund) :
    Factory::Factory(uniqueId, fund, ItemType::Plastic, {ItemType::Petrol}) {}

ChipFactory::ChipFactory(int uniqueId, int fund) :
    Factory::Factory(uniqueId, fund, ItemType::Chip, {ItemType::Sand, ItemType::Copper}) {}

RobotFactory::RobotFactory(int uniqueId, int fund) :
    Factory::Factory(uniqueId, fund, ItemType::Robot, {ItemType::Chip, ItemType::Plastic}) {}
