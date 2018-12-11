#include <eosiolib/eosio.hpp>
#include <eosiolib/time.hpp>
#include <eosiolib/crypto.h>
#include <eosiolib/asset.hpp>

#include "../Orders/Orders.hpp"

using namespace eosio;
using std::string;

CONTRACT Market : public eosio::contract
{
    using contract::contract;

  public:
    Market(name receiver, name code, datastream<const char *> ds) : contract(receiver, code, ds),
                                                                    _places(receiver, receiver.value),
                                                                    _assignments(receiver, receiver.value),
                                                                    _offers(receiver, receiver.value),
                                                                    _applies(receiver, receiver.value),
                                                                    _orders(name("rideor"), name("rideor").value),
                                                                    _users(name("rideos"), name("rideos").value) {}

    ACTION addplace(string & country, string & zipCode);

    ACTION updateplace(uint64_t key, string & country, string & zipCode);

    ACTION deleteplace(uint64_t key);

    ACTION newassign(name account, uint64_t placeKey);

    ACTION endassign(uint64_t assignmentKey);

    ACTION addoffer(uint64_t orderKey, uint64_t placeKey);

    ACTION endoffer(name deliver, uint64_t offerKey);

    ACTION canceloffer(uint64_t offerKey);

    ACTION addapply(name account, uint64_t offerKey);

    ACTION cancelapply(uint64_t applyKey);

    TABLE place
    {
        uint64_t placeKey;
        string country;
        string zipCode;

        uint64_t primary_key() const { return placeKey; }
    };
    typedef multi_index<name("place"), place> place_table;

    TABLE assignment
    {
        uint64_t assignmentKey;
        uint64_t placeKey;
        name account;
        time_point_sec endAssignment;

        uint64_t primary_key() const { return assignmentKey; }
        uint64_t get_account() const { return account.value; }
        uint64_t get_place_key() const { return placeKey; }
    };
    typedef multi_index<name("assignment"), assignment,
                        indexed_by<name("byuserkey"),
                                   const_mem_fun<assignment, uint64_t, &assignment::get_account>>,
                        indexed_by<name("byplacekey"),
                                   const_mem_fun<assignment, uint64_t, &assignment::get_place_key>>>
        assignment_table;

    enum offer_state : uint8_t
    {
        OPEN = 0,
        CLOSED = 1,
        FOUNDED = 2,
    };

    TABLE offer
    {
        uint64_t offerKey;
        uint64_t orderKey;
        uint64_t placeKey;
        uint8_t stateOffer;

        uint64_t primary_key() const { return offerKey; }
        uint64_t getOrderKey() const { return orderKey; }
        uint64_t getPlaceKey() const { return placeKey; }
    };
    typedef multi_index<name("offer"), offer,
                        indexed_by<name("byorderkey"),
                                   const_mem_fun<offer, uint64_t, &offer::getOrderKey>>,
                        indexed_by<name("byplacekey"),
                                   const_mem_fun<offer, uint64_t, &offer::getPlaceKey>>>
        offer_table;

    TABLE apply
    {
        uint64_t applyKey;
        name deliver;
        uint64_t offerKey;

        uint64_t primary_key() const { return applyKey; }
        uint64_t get_offer() const { return offerKey; }
    };
    typedef multi_index<name("apply"), apply,
                        indexed_by<name("byoffer"),
                                   const_mem_fun<apply, uint64_t, &apply::get_offer>>>
        apply_table;

  private:
    Orders::order_table _orders;
    Users::user_table _users;
    place_table _places;
    assignment_table _assignments;
    offer_table _offers;
    apply_table _applies;
};