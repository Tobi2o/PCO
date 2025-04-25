/**
 * @file extractor.cpp
 * @brief En-tête pour la classe Extractor avec des modifications pour la synchronisation.
 * - Se  situe normalement danns le .h mais exceptionnellement ici pour ce laboratoire.
 * @date 2023-11-08
 * @author Christen Anthony, Harun Ouweis
 *
 * Historique des modifications :
 * - Ajout de la gestion de la concurrence dans les méthodes `trade` et `run`
 *   pour protéger les ressources partagées en utilisant un mutex.
 * - Intégration d'une condition d'arrêt propre dans la méthode `run` pour permettre
 *   une terminaison gracieuse du thread.
 */

#include "extractor.h"
#include "costs.h"
#include <pcosynchro/pcothread.h>

WindowInterface* Extractor::interface = nullptr;

Extractor::Extractor(int uniqueId, int fund, ItemType resourceExtracted)
    : Seller(fund, uniqueId), resourceExtracted(resourceExtracted), nbExtracted(0)
{
    assert(resourceExtracted == ItemType::Copper ||
           resourceExtracted == ItemType::Sand ||
           resourceExtracted == ItemType::Petrol);
    interface->consoleAppendText(uniqueId, QString("Mine Created"));
    interface->updateFund(uniqueId, fund);
}

std::map<ItemType, int> Extractor::getItemsForSale() {
    return stocks;
}


int Extractor::trade(ItemType it, int qty) {
    if (qty <= 0 || it != getResourceMined()) {
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

void Extractor::run() {
    interface->consoleAppendText(uniqueId, "[START] Mine routine");

    while (!PcoThread::thisThread()->stopRequested()) {
        int minerCost = getEmployeeSalary(getEmployeeThatProduces(resourceExtracted));

        resourcesProtector.lock(); // Début S.C.
        if (money < minerCost) {
            resourcesProtector.unlock(); // Fin S.C.
            /* Pas assez d'argent */
            /* Attend des jours meilleurs */
            PcoThread::usleep(1000U);
            continue;
        }

        /* On peut payer un mineur */
        money -= minerCost;
        resourcesProtector.unlock(); // Fin S.C.

        /* Temps aléatoire borné qui simule le mineur qui mine */
        PcoThread::usleep((rand() % 100 + 1) * 10000);
        /* Statistiques */
        nbExtracted++;
        /* Incrément des stocks */
        resourcesProtector.lock(); // Début S.C.
        ++stocks[resourceExtracted];
        resourcesProtector.unlock(); // Fin S.C.
        /* Message dans l'interface graphique */
        interface->consoleAppendText(uniqueId, QString("1 ") % getItemName(resourceExtracted) %
                                     " has been mined");
        /* Update de l'interface graphique */
        interface->updateFund(uniqueId, money);
        interface->updateStock(uniqueId, &stocks);
    }
    interface->consoleAppendText(uniqueId, "[STOP] Mine routine");
}

int Extractor::getMaterialCost() {
    return getCostPerUnit(resourceExtracted);
}

ItemType Extractor::getResourceMined() {
    return resourceExtracted;
}

int Extractor::getAmountPaidToMiners() {
    return nbExtracted * getEmployeeSalary(getEmployeeThatProduces(resourceExtracted));
}

void Extractor::setInterface(WindowInterface *windowInterface) {
    interface = windowInterface;
}

SandExtractor::SandExtractor(int uniqueId, int fund): Extractor::Extractor(uniqueId, fund, ItemType::Sand) {}

CopperExtractor::CopperExtractor(int uniqueId, int fund): Extractor::Extractor(uniqueId, fund, ItemType::Copper) {}

PetrolExtractor::PetrolExtractor(int uniqueId, int fund): Extractor::Extractor(uniqueId, fund, ItemType::Petrol) {}
