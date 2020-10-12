#include <eosio/eosio.hpp>
#include <eosio/asset.hpp>

namespace sx {

using eosio::asset;
using eosio::name;
using eosio::symbol_code;
using eosio::time_point_sec;

using std::map;

class [[eosio::contract("stats.sx")]] stats : public contract {

public:
    using contract::contract;

    /**
     * ## TABLE `volume`
     *
     * - `{name} contract` - contract name
     * - `{time_point_sec} timestamp` - daily periods (86400 seconds)
     * - `{uint64_t} transactions` - total amount of transactions
     * - `{map<symbol_code, asset>} fees` - total fees collected
     * - `{map<symbol_code, asset>} volume` - total trading volume of assets
     *
     * ### example
     *
     * ```json
     * {
     *     "contract": "swap.sx",
     *     "timestamp": "2020-06-03T00:00:00",
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
        time_point_sec                  timestamp;
        uint64_t                        transactions;
        map<symbol_code, asset>         volume;
        map<symbol_code, asset>         fees;

        uint64_t primary_key() const { return contract.value; }
    };
    typedef eosio::multi_index< "volume"_n, volume_row > volume;

    /**
     * ## TABLE `spotprices`
     *
     * - `{name} contract` - contract name
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
     * Notify contract when any token transfer notifiers relay contract
     */
    [[eosio::on_notify("*::swaplog")]]
    void on_swaplog( const name buyer,
                     const asset amount_in,
                     const asset amount_out,
                     const asset fee );
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