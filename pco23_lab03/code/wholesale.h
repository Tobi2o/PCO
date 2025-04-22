#ifndef WHOLESALE_H
#define WHOLESALE_H
#include "seller.h"
#include <vector>
#include "windowinterface.h"

/**
 * @brief La classe permet l'implémentation d'un grossiste et de ces fonctions
 *        de ventes et d'achats.
 */
class Wholesale : public Seller
{
private:
    // Vecteur de vendeurs (mines, usines) auxquels le grossiste peut acheter des ressources
    std::vector<Seller*> sellers;

    static WindowInterface* interface;

    /**
     * @brief Tente d'acheter des ressources auprès d'un vendeur aléatoire.
     *
     * Cette fonction choisit un vendeur et une ressource au hasard et tente d'acheter une quantité aléatoire de cette ressource.
     * Elle vérifie si les fonds sont suffisants avant de procéder à l'achat. Les sections critiques sont protégées par un mutex
     * pour s'assurer que l'accès aux fonds et aux stocks est thread-safe.
     *
     */
    void buyResources();
public:
    /**
     * @brief Constructeur de grossiste
     * @param Fond du grossiste à sa création
     */
    Wholesale(int uniqueId, int fund);

    /**
     * @brief Routine principale du grossiste exécutée par le thread du grossiste.
     *
     * Cette fonction exécute une boucle qui tente continuellement d'acheter des ressources et met à jour les fonds
     * et les stocks du grossiste. La boucle se termine proprement lorsque l'arrêt du thread est demandé.
     */
    void run();

    std::map<ItemType, int> getItemsForSale() override;
    /**
     * @brief Effectue une transaction de vente des ressources du grossiste.
     *
     * Cette fonction gère la vente de ressources. Elle vérifie si la quantité demandée est disponible et si oui,
     * elle décrémente les stocks de la ressource et incrémente les fonds du grossiste de manière thread-safe.
     *
     * @param it Le type de ressource à vendre.
     * @param qty La quantité de ressource à vendre.
     * @return int Le revenu généré par la vente si la transaction est réussie, sinon 0.
     */
    int trade(ItemType it, int qty) override;

    /**
     * @brief Fonction permettant de lier des vendeurs
     * @param Vecteurs
     */
    void setSellers(std::vector<Seller*> sellers);

    static void setInterface(WindowInterface* windowInterface);
};

#endif // WHOLESALE_H
