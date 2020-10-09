#include <eosio/eosio.hpp>
#include <eosio/singleton.hpp>
#include <eosio/asset.hpp>

using namespace eosio;
using namespace std;

namespace sx {

class [[eosio::contract("stats.sx")]] stats : public contract {

public:
    using contract::contract;

    /**
     * ## TABLE `volume`
     *
     * - `{name} contract` - contract name
     * - `{time_point_sec} timestamp` - daily periods (86400 seconds)
     * - `{map<symbol_code, asset>} fees` - total fees collected
     * - `{map<symbol_code, asset>} volume` - total trading volume of assets
     *
     * ### example
     *
     * ```json
     * {
     *     "contract": "swap.sx",
     *     "timestamp": "2020-06-03T00:00:00",
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
    struct [[eosio::table("volume")]] volume_params {
        // name                        contract;
        time_point_sec              timestamp;
        map<symbol_code, asset>     volume;
        map<symbol_code, asset>     fees;

        // uint64_t primary_key() const { return contract.value; }
    };
    typedef eosio::singleton< "volume"_n, volume_params > volume;

    /**
     * Notify contract when any token transfer notifiers relay contract
     */
    [[eosio::on_notify("*::log")]]
    void on_log( const name buyer,
                 const asset quantity,
                 const asset rate,
                 const asset fee );
private:
    void update_volume( const vector<asset> volumes, const asset fee );
};
}