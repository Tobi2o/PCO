#ifndef EXTRACTOR_H
#define EXTRACTOR_H
#include <QTimer>
#include "windowinterface.h"
#include "costs.h"
#include "seller.h"

/**
 * @brief La classe offrant l'implémentation d'une mine et ces fonctions de ventes.
 */
class Extractor : public Seller {
public:
    static void setInterface(WindowInterface* interface);

    /**
     * @brief Constructeur d'une mine
     * @param Fonds d'initialisation de la mine
     * @param Ressource minée
     */
    Extractor(int uniqueId, int fund, ItemType resourceExtracted);

    std::map<ItemType, int> getItemsForSale() override;

    /**
     * @brief Effectue une transaction de vente de ressources.
     *
     * Cette fonction gère la vente d'une quantité spécifiée de la ressource extraite. Elle vérifie d'abord si la
     * transaction est possible (quantité positive et ressource correspondante), puis procède à la transaction
     * en modifiant les stocks et en augmentant les fonds de l'extracteur de manière thread-safe.
     *
     * @param it Le type de ressource à vendre.
     * @param qty La quantité de ressource à vendre.
     * @return int Le revenu généré par la vente si la transaction est réussie, sinon 0.
     */
    int trade(ItemType it, int qty) override;

    /**
     * @brief Routine principale d'extraction exécutée par le thread de l'extracteur.
     *
     * Cette fonction simule le processus d'extraction en continu jusqu'à ce qu'un signal d'arrêt soit détecté.
     * Elle gère le paiement des coûts de main-d'œuvre, l'extraction des ressources, la mise à jour des statistiques,
     * et l'interaction avec l'interface utilisateur, tout en assurant que ces opérations sont sécurisées pour l'exécution
     * concurrente.
     *
     * La boucle principale vérifie si l'extracteur a suffisamment de fonds pour payer le mineur. Si ce n'est pas le cas,
     * l'extracteur attend et réessaie. Si les fonds sont suffisants, l'extracteur paie le mineur, mine la ressource,
     * et met à jour les stocks et les fonds de manière sécurisée
     */
    void run();

    /**
     * @brief Fonction permettant de savoir quelle ressources la mine possède
     * @return Le type de minerai minés
     */
    ItemType getResourceMined();

    /**
     * @brief Fonction permettant de savoir le coût de la ressources minée
     * @return Le coût de la ressource minée
     */
    int getMaterialCost();

    int getAmountPaidToMiners();

private:
    // Identifiant du type de ressourcee miné
    const ItemType resourceExtracted;
    // Compte le nombre d'employé payé
    int nbExtracted;

    static WindowInterface* interface;
};


// Si dessous sont les trois types de mines. Elle vont créer des mines en leur
// spécifiant leur identifiant.

class SandExtractor : public Extractor {
private:
public:
    SandExtractor(int uniqueId, int fund);
};


class CopperExtractor : public Extractor {
private:
public:
    CopperExtractor(int uniqueId, int fund);
};


class PetrolExtractor : public Extractor {
private:
public:
    PetrolExtractor(int uniqueId, int fund);
};




#endif // EXTRACTOR_H
