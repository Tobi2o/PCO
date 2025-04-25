/**
 * @file wholesale.cpp
 * @brief En-tête pour la classe Wholesale avec des modifications pour la synchronisation et la gestion des transactions.
 * - Se  situe normalement danns le .h mais exceptionnellement ici pour ce laboratoire.
 * @date 2023-11-08
 * @author Christen Anthony, Harun Ouweis
 *
 * Historique des modifications :
 * - Ajout de la gestion de la concurrence dans les méthodes `buyResources` et `trade`
 *   pour protéger l'accès aux fonds et aux stocks à l'aide d'un mutex.
 * - Implémentation de la terminaison propre des threads dans la méthode `run`
 *   via l'utilisation de `PcoThread::thisThread()->stopRequested()`.
 * - Extension de la méthode `trade` pour inclure la logique de transaction complète,
 *   y compris la mise à jour de l'interface utilisateur après une vente.
 */

#include "wholesale.h"
#include "factory.h"
#include "costs.h"
#include <iostream>
#include <pcosynchro/pcothread.h>

WindowInterface* Wholesale::interface = nullptr;

Wholesale::Wholesale(int uniqueId, int fund)
    : Seller(fund, uniqueId)
{
    interface->updateFund(uniqueId, fund);
    interface->consoleAppendText(uniqueId, "Wholesaler Created");

}

void Wholesale::setSellers(std::vector<Seller*> sellers) {
    this->sellers = sellers;

    for(Seller* seller: sellers){
        interface->setLink(uniqueId, seller->getUniqueId());
    }
}

void Wholesale::buyResources() {
    auto s = Seller::chooseRandomSeller(sellers);
    auto m = s->getItemsForSale();
    auto i = Seller::chooseRandomItem(m);

    if (i == ItemType::Nothing) {
        /* Nothing to buy... */
        return;
    }

    int qty = rand() % 5 + 1;
    int price = qty * getCostPerUnit(i);

    interface->consoleAppendText(uniqueId, QString("I would like to buy %1 of ").arg(qty) %
                                 getItemName(i) % QString(" which would cost me %1").arg(price));

    resourcesProtector.lock(); // Début S.C.
    if (money < price) {
        resourcesProtector.unlock(); // Fin S.C.
        return;
    }
    resourcesProtector.unlock(); // Fin S.C.

    int bill = s->trade(i, qty);

    if (bill == 0) {
        return;
    }

    resourcesProtector.lock(); // Début S.C.
    money -= bill;
    stocks[i] += qty;
    resourcesProtector.unlock(); // Fin S.C.
}


void Wholesale::run() {

    if (sellers.empty()) {
        std::cerr << "You have to give factories and mines to a wholeseler before launching is routine" << std::endl;
        return;
    }

    interface->consoleAppendText(uniqueId, "[START] Wholesaler routine");
    while (!PcoThread::thisThread()->stopRequested()) {
        buyResources();
        interface->updateFund(uniqueId, money);
        interface->updateStock(uniqueId, &stocks);
        //Temps de pause pour espacer les demandes de ressources
        PcoThread::usleep((rand() % 10 + 1) * 100000);
    }
    interface->consoleAppendText(uniqueId, "[STOP] Wholesaler routine");


}

std::map<ItemType, int> Wholesale::getItemsForSale() {
    return stocks;
}

int Wholesale::trade(ItemType it, int qty) {
    bool isAnItemForSale = false;

    for(auto item : getItemsForSale()) {
        if (it == item.first) {
            isAnItemForSale = true;
            break;
        }
    }

    if (qty <= 0 || !isAnItemForSale) {
        return 0;
    }

    resourcesProtector.lock(); // Début S.C.
    if (stocks[it] < qty) {
        resourcesProtector.unlock(); // Fin S.C.
        return 0;
    }
    stocks[it] -= qty;
    money += getCostPerUnit(it) * qty;
    resourcesProtector.unlock(); // Fin S.C.

    interface->consoleAppendText(uniqueId, QString("I sold %1 ").arg(qty) % getItemName(it) % QString(" wich brought me %1").arg(getCostPerUnit(it) * qty));

    interface->updateFund(uniqueId, money);
    interface->updateStock(uniqueId, &stocks);

    return getCostPerUnit(it) * qty;
}

void Wholesale::setInterface(WindowInterface *windowInterface) {
    interface = windowInterface;
}
