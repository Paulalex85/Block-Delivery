#include <eosiolib/eosio.hpp>
#include <eosiolib/print.hpp>
#include <string>

using namespace eosio;
namespace rideEOS {

    using namespace eosio;
    using std::string;

    class Orders : public contract{
        using contract::contract;

    public:
        Orders(account_name self):contract(self) {}

        /*enum OrderStatut {
            Initialization,
            WaitBiker,
            WaitSeller,
            SellerPreparation,
            OrderReady,
            OrderTaken,
            OrderDelivered,
            OrderCancel
        };*/

        //@abi action
        void initialize(account_name buyer, account_name seller, account_name deliver);

        //@abi action
        void addinkart(uint64_t orderKey,account_name buyer,account_name seller, uint64_t productKey, uint64_t quantity);

        //@abi action
        void deleteinkart(uint64_t orderKey, account_name buyer, uint64_t productKey);

        //@abi action
        void getorder(const uint64_t orderKey);

        //@abi action
        void getorderbyse(const account_name seller);

        //@abi action
        void getorderbybu(const account_name buyer);

        //@abi action
        void getorderbyde(const account_name deliver);

        struct kart {
            uint64_t productKey;
            uint64_t quantity;
            uint64_t price;
        };

        //@abi table order i64
        struct order {
            uint64_t orderKey;
            account_name buyer;
            account_name seller;
            account_name deliver;
            uint64_t statut;
            uint64_t date;//TODO change type
            vector<kart> karts;

            uint64_t primary_key() const { return orderKey; }
            account_name get_buyer_key() const { return buyer; }
            account_name get_seller_key() const { return seller; }
            account_name get_deliver_key() const { return deliver; }

            EOSLIB_SERIALIZE(order, (orderKey)(buyer)(seller)(deliver)(statut)(date)(karts))
        };

        typedef multi_index<N(orders), order,
            indexed_by < N(bybuyerkey),
                    const_mem_fun <order, account_name, &order::get_buyer_key>
            >,
            indexed_by < N(bysellerkey),
                    const_mem_fun <order, account_name, &order::get_seller_key>
            >,
            indexed_by < N(bydeliverkey),
                    const_mem_fun <order, account_name, &order::get_deliver_key>
            >
        > orderIndex;
    };
}
