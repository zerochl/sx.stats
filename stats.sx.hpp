#include <eosio/eosio.hpp>
#include <eosio/asset.hpp>

namespace sx {

using eosio::asset;
using eosio::name;
using eosio::symbol_code;
using eosio::time_point_sec;
using eosio::contract;

using std::map;
using std::vector;

class [[eosio::contract("stats.sx")]] stats : public contract {

public:
    using contract::contract;

    /**
     * ## TABLE `volume`
     *
     * - `{name} contract` - (primary key) contract name
     * - `{time_point_sec} last_modified` - last modified timestamp
     * - `{uint64_t} transactions` - total amount of transactions
     * - `{map<symbol_code, asset>} fees` - total fees collected
     * - `{map<symbol_code, asset>} volume` - total trading volume of assets
     *
     * ### example
     *
     * ```json
     * {
     *     "contract": "swap.sx",
     *     "last_modified": "2020-06-03T00:00:00",
     *     "transactions": 110,
     *     "volume": [
     *         {"key": "EOS", "value": "25.0000 EOS"},
     *         {"key": "USDT", "value": "100.0000 USDT"}
     *     ],
     *     "fees": [
     *         {"key": "EOS", "value": "0.1250 EOS"},
     *         {"key": "USDT", "value": "0.5000 USDT"}
     *     ]
     * }
     * ```
     */
    struct [[eosio::table("volume")]] volume_row {
        name                            contract;
        time_point_sec                  last_modified;
        uint64_t                        transactions;
        map<symbol_code, asset>         volume;
        map<symbol_code, asset>         fees;

        uint64_t primary_key() const { return contract.value; }
    };
    typedef eosio::multi_index< "volume"_n, volume_row > volume;

    /**
     * ## TABLE `flash`
     *
     * - `{name} contract` - (primary key) contract name
     * - `{time_point_sec} last_modified` - last modified timestamp
     * - `{uint64_t} transactions` - total amount of transactions
     * - `{map<symbol_code, asset>} borrow` - total borrowed asset from flash contract
     * - `{map<symbol_code, asset>} fees` - total fees collected
     * - `{map<symbol_code, asset>} reserves` - total reserve assets of flash contract
     *
     * ### example
     *
     * ```json
     * {
     *     "contract": "flash.sx",
     *     "last_modified": "2020-06-03T00:00:00",
     *     "transactions": 110,
     *     "borrow": [
     *         {"key": "EOS", "value": "2500.0000 EOS"},
     *         {"key": "USDT", "value": "100.0000 USDT"}
     *     ],
     *     "fees": [
     *         {"key": "EOS", "value": "0.1250 EOS"},
     *         {"key": "USDT", "value": "0.0100 USDT"}
     *     ],
     *     "reserves": [
     *         {"key": "EOS", "value": "900.0000 EOS"},
     *         {"key": "USDT", "value": "200.0000 USDT"}
     *     ]
     * }
     * ```
     */
    struct [[eosio::table("flash")]] flash_row {
        name                            contract;
        time_point_sec                  last_modified;
        uint64_t                        transactions;
        map<symbol_code, asset>         borrow;
        map<symbol_code, asset>         fees;
        map<symbol_code, asset>         reserves;

        uint64_t primary_key() const { return contract.value; }
    };
    typedef eosio::multi_index< "flash"_n, flash_row > flash;

    /**
     * ## TABLE `spotprices`
     *
     * - `{name} contract` - (primary key) contract name
     * - `{time_point_sec} last_modified` - last modified timestamp
     * - `{symbol_code} base` - base symbol code
     * - `{map<symbol_code, double>} quotes` - quotes prices calculated relative to base
     *
     * ### example
     *
     * ```json
     * {
     *   "contract": "swap.sx",
     *   "last_modified": "2020-07-10T15:17:23",
     *   "base": "USDT",
     *   "quotes": [
     *     {"key": "EOS", "value": 2.6098},
     *     {"key": "USDT", "value": 1.0000}
     *   ]
     * }
     * ```
     */
    struct [[eosio::table("spotprices")]] spotprices_row {
        name                        contract;
        time_point_sec              last_modified;
        symbol_code                 base;
        map<symbol_code, double>    quotes;

        uint64_t primary_key() const { return contract.value; }
    };
    typedef eosio::multi_index< "spotprices"_n, spotprices_row > spotprices;

    /**
     * ## TABLE `trades`
     *
     * - `{name} contract` - (primary key) contract name
     * - `{time_point_sec} last_modified` - last modified timestamp
     * - `{uint64_t} transactions` - total amount of transactions
     * - `{map<symbol_code, asset>} quantities` - total quantity traded
     * - `{map<name, uint64_t>} codes` - total transactions per contract codes
     * - `{map<symbol_code, uint64_t>} symbcodes` - total transactions per symbol code used
     * - `{map<name, uint64_t>} executors` - total transactions per executor
     * - `{map<symbol_code, asset>} profits` - total profits
     *
     * ### example
     *
     * ```json
     * {
     *     "contract": "basic.sx",
     *     "last_modified": "2020-06-03T00:00:00",
     *     "transactions": 640,
     *     "borrow": [
     *         {"key": "EOS", "value": "49387.8252 EOS"}
     *     ],
     *     "quantities": [
     *         {"key": "EOS", "value": "5030.3050 EOS"},
     *         {"key": "USDT", "value": "400.0100 USDT"}
     *     ],
     *     "codes": [
     *         {"key": "swap.defi", "value": 512},
     *         {"key": "swap.sx", "value": 100}
     *     ],
     *     "symcodes": [
     *         {"key": "EOS", "value": 610},
     *         {"key": "USDT", "value": 30}
     *     ],
     *     "executors": [
     *         {"key": "myaccount", "value": 80},
     *         {"key": "miner.sx", "value": 200}
     *     ],
     *     "profits": [
     *         {"key": "EOS", "value": "50.3050 EOS"},
     *         {"key": "USDT", "value": "4.0100 USDT"}
     *     ]
     * }
     * ```
     */
    struct [[eosio::table("trades")]] trades_row {
        name                            contract;
        time_point_sec                  last_modified;
        uint64_t                        transactions;
        map<symbol_code, asset>         borrow;
        map<symbol_code, asset>         quantities;
        map<name, uint64_t>             codes;
        map<symbol_code, uint64_t>      symcodes;
        map<name, uint64_t>             executors;
        map<symbol_code, asset>         profits;

        uint64_t primary_key() const { return contract.value; }
    };
    typedef eosio::multi_index< "trades"_n, trades_row > trades;

    [[eosio::action]]
    void erase( const name contract );

    /**
     * ## ACTION `swaplog`
     *
     * Notify of trade
     *
     * - **authority**: `get_self()`
     *
     * ### params
     *
     * - `{name} buyer` - trader buyer account
     * - `{asset} amount_in` - amount incoming
     * - `{asset} amount_out` - amount outgoing
     * - `{asset} fee` - fee paid
     *
     * ### example
     *
     * ```bash
     * cleos push action swap.sx swaplog '["myaccount", "3.0000 EOS", "7.0486 USDT", "0.0060 EOS"]' -p swap.sx
     * ```
     */
    [[eosio::action]]
    void swaplog( const name contract, const name buyer, const asset amount_in, const asset amount_out, const asset fee );

    [[eosio::action]]
    void flashlog( const name contract, const name receiver, const asset borrow, const asset fee, const asset reserve );

    [[eosio::action]]
    void tradelog( const name contract, const name executor, const asset borrow, const vector<asset> quantities, const vector<name> codes, const asset profit );

    // action wrappers
    using swaplog_action = eosio::action_wrapper<"swaplog"_n, &sx::stats::swaplog>;
    using flashlog_action = eosio::action_wrapper<"flashlog"_n, &sx::stats::flashlog>;
    using tradelog_action = eosio::action_wrapper<"tradelog"_n, &sx::stats::tradelog>;

private:
    // volume
    void update_volume( const name contract, const vector<asset> volumes, const asset fee );

    // spotprices
    void update_spot_prices( const name contract );
    double get_spot_price( const name contract, const symbol_code base, const symbol_code quote );
    map<symbol_code, double> get_spot_prices( const name contract, const symbol_code base );
    bool is_token_exists( const name contract, const symbol_code symcode );
};
}