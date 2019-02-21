#pragma once

class user_action
{
  private:
    name _self;
    user_table _users;
    stackpower_table _stackpower;

  public:
    user_action(name receiver) : _self(receiver), _users(receiver, receiver.value), _stackpower(receiver, receiver.value) {}

    void adduser(name account, string &username)
    {
        require_auth(account);

        auto iterator = _users.find(account.value);
        eosio_assert(iterator == _users.end(), "Address for account already exists");

        _users.emplace(_self, [&](auto &user) {
            user.account = account;
            user.username = username;
            user.balance = eosio::asset(0, symbol(symbol_code("SYS"), 4));
        });
    }

    void updateuser(name account, string &username)
    {
        require_auth(account);

        auto iterator = _users.find(account.value);
        eosio_assert(iterator != _users.end(), "Address for account not found");

        _users.modify(iterator, _self, [&](auto &user) {
            user.username = username;
        });
    }

    void deposit(const name account, const asset &quantity)
    {
        eosio_assert(quantity.is_valid(), "invalid quantity");
        eosio_assert(quantity.amount > 0, "must deposit positive quantity");

        auto iterator = _users.find(account.value);
        eosio_assert(iterator != _users.end(), "Address for account not found");

        action(
            permission_level{account, name("active")},
            name("eosio.token"),
            name("transfer"),
            std::make_tuple(account, _self, quantity, std::string("")))
            .send();

        _users.modify(iterator, _self, [&](auto &user) {
            user.balance += quantity;
        });
    }

    void withdraw(const name account, const asset &quantity)
    {
        require_auth(account);

        eosio_assert(quantity.is_valid(), "invalid quantity");
        eosio_assert(quantity.amount > 0, "must withdraw positive quantity");

        auto iterator = _users.find(account.value);
        eosio_assert(iterator != _users.end(), "Address for account not found");

        _users.modify(iterator, _self, [&](auto &user) {
            eosio_assert(user.balance >= quantity, "insufficient balance");
            user.balance -= quantity;
        });

        action(
            permission_level{_self, name("active")},
            name("eosio.token"), name("transfer"),
            std::make_tuple(_self, account, quantity, std::string("")))
            .send();
    }

    void pay(const name account, const name receiver, const asset &quantity)
    {
        require_auth(account);

        eosio_assert(quantity.is_valid(), "invalid quantity");
        eosio_assert(quantity.amount > 0, "must withdraw positive quantity");
        eosio_assert(quantity.symbol == eosio::symbol("SYS", 4), "only core token allowed");

        auto iterator = _users.find(account.value);
        eosio_assert(iterator != _users.end(), "Address for account not found");

        _users.modify(iterator, _self, [&](auto &user) {
            eosio_assert(user.balance >= quantity, "insufficient balance");
            user.balance -= quantity;
        });

        action(
            permission_level{_self, name("active")},
            name("eosio.token"), name("transfer"),
            std::make_tuple(_self, receiver, quantity, std::string("")))
            .send();
    }

    void receive(const name account, const name from, const asset &quantity)
    {
        require_auth(from);

        eosio_assert(quantity.is_valid(), "invalid quantity");
        eosio_assert(quantity.amount > 0, "must withdraw positive quantity");
        eosio_assert(quantity.symbol == eosio::symbol("SYS", 4), "only core token allowed");

        auto iterator = _users.find(account.value);
        eosio_assert(iterator != _users.end(), "Address for account not found");

        _users.modify(iterator, _self, [&](auto &user) {
            user.balance += quantity;
        });
    }

    void stackpow(const name account, const asset &quantity, const uint64_t placeKey)
    {
        require_auth(account);

        eosio_assert(quantity.is_valid(), "invalid quantity");
        eosio_assert(quantity.amount > 0, "must withdraw positive quantity");
        eosio_assert(quantity.symbol == eosio::symbol("SYS", 4), "only core token allowed");

        auto iteratorUser = _users.find(account.value);
        eosio_assert(iteratorUser != _users.end(), "Address for account not found");

        // Market::place_table _places(name("rideom"), name("rideom").value);
        // auto iteratorPlace = _places.find(placeKey);
        // eosio_assert(iteratorPlace != _places.end(), "Place not found");
        // eosio_assert(iteratorPlace->active == true, "Place is not active");

        // eosio_assert(iteratorUser->balance >= quantity, "Insufficient balance");

        // _users.modify(iteratorUser, _self, [&](auto &user) {
        //     user.balance -= quantity;
        // });

        // auto indexStack = _stackpower.get_index<name("byaccount")>();
        // auto iteratorStack = indexStack.find(account.value);

        // bool found = false;

        // while (iteratorStack != indexStack.end())
        // {
        //     if (iteratorStack->placeKey == placeKey && iteratorStack->account == account)
        //     {
        //         indexStack.modify(iteratorStack, _self, [&](auto &stackpower) {
        //             stackpower.balance += quantity;
        //         });
        //         found = true;
        //         break;
        //     }
        //     else
        //     {
        //         iteratorStack++;
        //     }
        // }

        // if (!found)
        // {
        //     _stackpower.emplace(_self, [&](auto &stackpower) {
        //         stackpower.idStackPower = _stackpower.available_primary_key();
        //         stackpower.account = account;
        //         stackpower.balance = quantity;
        //         stackpower.placeKey = placeKey;
        //     });
        // }
    }

    void unstackpow(const name account, const asset &quantity, const uint64_t stackKey)
    {
        require_auth(account);

        eosio_assert(quantity.is_valid(), "invalid quantity");
        eosio_assert(quantity.amount > 0, "must withdraw positive quantity");
        eosio_assert(quantity.symbol == eosio::symbol("SYS", 4), "only core token allowed");

        auto iteratorUser = _users.find(account.value);
        eosio_assert(iteratorUser != _users.end(), "Address for account not found");

        auto iteratorStackpower = _stackpower.find(stackKey);
        eosio_assert(iteratorStackpower != _stackpower.end(), "Address for stackpower not found");

        eosio_assert(iteratorStackpower->balance >= quantity, "The amount should be less or equal than the balance");
        eosio_assert(iteratorStackpower->account == account, "The user is not the same");

        if (iteratorStackpower->balance == quantity)
        {
            _stackpower.erase(iteratorStackpower);
        }
        else
        {
            _stackpower.modify(iteratorStackpower, _self, [&](auto &stackpower) {
                stackpower.balance -= quantity;
            });
        }

        _users.modify(iteratorUser, _self, [&](auto &user) {
            user.balance += quantity;
        });
    }
};