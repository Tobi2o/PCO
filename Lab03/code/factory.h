#ifndef FACTORY_H
#define FACTORY_H
#include <vector>
#include "windowinterface.h"
#include "seller.h"

class Wholesale;

/**
 * @brief La classe permet l'implémentation d'une usine et de ces fonctions
 *        de ventes et d'achats.
 */
class Factory : public Seller
{
public:
    /**
     * @brief Constructeur de la classe Factory
     * @param Fonds initiale
     * @param La ressources qui sera construite par l'usine
     * @param Vecteurs de ressources nécessaires pour construire l'objet
     */
    Factory(int uniqueId, int fund, ItemType builtItem, std::vector<ItemType> resourcesNeeded);

    /**
     * @brief Routine principale de l'usine exécutée par le thread de l'usine.
     *
     * Cette fonction exécute une boucle qui vérifie continuellement les ressources et déclenche la construction d'objets
     * ou passe des commandes de ressources en fonction des besoins. La boucle se termine proprement lorsque l'arrêt du thread
     * est demandé.
     */
    void run();

    std::map<ItemType, int> getItemsForSale() override;

    /**
     * @brief Effectue une transaction de vente de l'objet construit par l'usine.
     *
     * Cette fonction gère la vente de l'objet construit. Elle vérifie si la quantité demandée est disponible et si oui,
     * elle décrémente les stocks de l'objet et incrémente les fonds de l'usine, en s'assurant que ces opérations sont
     * thread-safe grâce à l'utilisation de mutex pour encadrer les sections critiques.
     *
     * @param it Le type d'objet à vendre.
     * @param qty La quantité d'objets à vendre.
     * @return int Le revenu généré par la vente si la transaction est réussie, sinon 0.
     */
    int trade(ItemType it, int number) override;

    /**
     * @brief Permet d'accèder au coût du matériel produit par l'usine
     * @return Le côût du metérial produit
     */
    int getMaterialCost();

    /**
     * @brief Retourne l'identifiant de l'objet produit par l'usine
     * @return Identifiant de l'usine.
     */
    ItemType getItemBuilt();

    /**
     * @brief Cette fonction permet d'affecter à une usine pluseurs grossistes pour pouvoir échanger avec eux.
     * @param Vecteur de wholesaler
     */
    void setWholesalers(std::vector<Wholesale*> wholesalers);

    int getAmountPaidToWorkers();

    static void setInterface(WindowInterface* windowInterface);

private:
    // Liste de grossiste auxquels l'usine peut acheter des ressources
    std::vector<Wholesale*> wholesalers;
    // Liste de ressources voulus pour la production d'un objet
    const std::vector<ItemType> resourcesNeeded;
    // Identifiant de l'objet produit par l'usine, selon l'enum ItemType
    const ItemType itemBuilt;
    // Compte le nombre d'employé payé
    int nbBuild;

    static WindowInterface* interface;

    /**
     * @brief Fonction privée permettant de vérifier si l'usine à toute les ressources
     *        nécessaire à la construction d'un objet.
     * @return true si elle a assez de ressources, false sinon.
     */
    bool verifyResources();

    /**
     * @brief Commande des ressources aux grossistes si les stocks sont insuffisants.
     *
     * Cette fonction itère sur les ressources nécessaires et, si les stocks sont à zéro, elle passe une commande auprès
     * des grossistes disponibles, en vérifiant que l'usine a suffisamment d'argent avant de passer la commande.
     * Après l'achat, elle met à jour les stocks et les fonds de l'usine. Les modifications des ressources et des fonds
     * sont protégées par des mutex pour éviter toute condition de concurrence.
     */
    void orderResources();

    /**
     * @brief Construit l'objet spécifié par l'usine.
     *
     * Cette fonction déclenche le processus de construction de l'objet après avoir vérifié et déduit les coûts des employés.
     * Elle décrémente ensuite les stocks des ressources nécessaires et incrémente les stocks de l'objet construit.
     * L'accès aux variables partagées telles que les stocks et les fonds est protégé par des mutex pour assurer
     * la synchronisation entre les threads.
     */
    void buildItem();
};


// Si dessous sont les trois types d'usines. Elle vont créer des usines en leur
// spécifiant les besoins en matérial et leur identifiant.

class PlasticFactory : public Factory{
private:
public:
    PlasticFactory(int uniqueId, int fund);
};

class ChipFactory : public Factory {
private:
public:
    ChipFactory(int uniqueId, int fund);
};

class RobotFactory : public Factory {
private:
public:
    RobotFactory(int uniqueId, int fund);
};

#endif // FACTORY_H
