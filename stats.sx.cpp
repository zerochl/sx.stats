#include <sx.swap/swap.sx.hpp>
#include <sx.utils/utils.hpp>

#include "stats.sx.hpp"

[[eosio::action]]
void sx::stats::swaplog( const name contract, const name buyer, const asset amount_in, const asset amount_out, const asset fee )
{
    if ( !has_auth("network.sx"_n )) require_auth( contract );
    check( contract.suffix() == "sx"_n, "contract must be *.sx account");

    update_volume( contract, vector<asset>{ amount_in, amount_out }, fee );
    update_spot_prices( contract );
}

[[eosio::action]]
void sx::stats::erase( const name contract )
{
    require_auth( get_self() );

    sx::stats::volume _volume( get_self(), get_self().value );
    sx::stats::spotprices _spotprices( get_self(), get_self().value );
    sx::stats::trades _trades( get_self(), get_self().value );

    auto volume = _volume.find( contract.value );
    auto spotprices = _spotprices.find( contract.value );
    auto trades = _trades.find( contract.value );

    check( volume != _volume.end() || spotprices != _spotprices.end() || trades != _trades.end(), "no contract available to erase");
    if ( volume != _volume.end() ) _volume.erase( volume );
    if ( spotprices != _spotprices.end() ) _spotprices.erase( spotprices );
    if ( trades != _trades.end() ) _trades.erase( trades );
}

void sx::stats::update_volume( const name contract, const vector<asset> volumes, const asset fee )
{
    sx::stats::volume _volume( get_self(), get_self().value );
    auto itr = _volume.find( contract.value );

    // initial variables
    map<symbol_code, asset> volume;
    map<symbol_code, asset> fees;

    // append current stats if exists
    if ( itr != _volume.end() ) {
        volume = itr->volume;
        fees = itr->fees;
    }

    // volume
    for ( const asset quantity : volumes ) {
        if ( volume.find( quantity.symbol.code() ) != volume.end() ) volume[ quantity.symbol.code() ] += quantity;
        else volume[ quantity.symbol.code() ] = quantity;
    }

    // fees
    if ( fees.find( fee.symbol.code() ) != fees.end() ) fees[ fee.symbol.code() ] += fee;
    else fees[ fee.symbol.code() ] = fee;

    // save table
    if ( itr == _volume.end() ) {
        _volume.emplace( get_self(), [&]( auto & row ) {
            row.contract = contract;
            row.last_modified = current_time_point();
            row.transactions = 1;
            row.volume = volume;
            row.fees = fees;
        });
    } else {
        _volume.modify( itr, same_payer, [&]( auto & row ) {
            row.last_modified = current_time_point();
            row.transactions += 1;
            row.volume = volume;
            row.fees = fees;
        });
    }
}

[[eosio::action]]
void sx::stats::flashlog( const name contract, const name receiver, const asset borrow, const asset fee, const asset reserve )
{
    require_auth( contract );
    check( contract.suffix() == "sx"_n, "contract must be *.sx account");

    sx::stats::flash _flash( get_self(), get_self().value );
    auto itr = _flash.find( contract.value );

    // initial variables
    map<symbol_code, asset> _borrow;
    map<symbol_code, asset> _fees;
    map<symbol_code, asset> _reserves;

    // append current stats if exists
    if ( itr != _flash.end() ) {
        _borrow = itr->borrow;
        _fees = itr->fees;
        _reserves = itr->reserves;
    }

    // reserves (replace with current)
    _reserves[ reserve.symbol.code() ] = reserve;

    // fees (add)
    if ( _fees.find( fee.symbol.code() ) != _fees.end() ) _fees[ fee.symbol.code() ] += fee;
    else _fees[ fee.symbol.code() ] = fee;

    // borrow (add)
    if ( _borrow.find( borrow.symbol.code() ) != _borrow.end() ) _borrow[ borrow.symbol.code() ] += borrow;
    else _borrow[ borrow.symbol.code() ] = borrow;

    // save table
    if ( itr == _flash.end() ) {
        _flash.emplace( get_self(), [&]( auto & row ) {
            row.contract = contract;
            row.last_modified = current_time_point();
            row.transactions = 1;
            row.borrow = _borrow;
            row.fees = _fees;
            row.reserves = _reserves;
        });
    } else {
        _flash.modify( itr, same_payer, [&]( auto & row ) {
            row.last_modified = current_time_point();
            row.transactions += 1;
            row.borrow = _borrow;
            row.fees = _fees;
            row.reserves = _reserves;
        });
    }
}

[[eosio::action]]
void sx::stats::tradelog( const name contract, const name executor, const asset borrow, const vector<asset> quantities, const vector<name> codes, const asset profit ) {
    require_auth( contract );
    check( contract.suffix() == "sx"_n, "contract must be *.sx account");

    sx::stats::trades _trades( get_self(), get_self().value );
    auto itr = _trades.find( contract.value );

    // initial variables
    map<symbol_code, asset>         _borrow;
    map<symbol_code, asset>         _quantities;
    map<name, uint64_t>             _codes;
    map<symbol_code, uint64_t>      _symcodes;
    map<name, uint64_t>             _executors;
    map<symbol_code, asset>         _profits;

    // append current stats if exists
    if ( itr != _trades.end() ) {
        _borrow = itr->borrow;
        _quantities = itr->quantities;
        _codes = itr->codes;
        _symcodes = itr->symcodes;
        _executors = itr->executors;
        _profits = itr->profits;
    }

    // borrow (add)
    if ( _borrow.find( borrow.symbol.code() ) != _borrow.end() ) _borrow[ borrow.symbol.code() ] += borrow;
    else _borrow[ borrow.symbol.code() ] = borrow;

    for ( const asset quantity : quantities ) {
        // quantities (add)
        if ( _quantities.find( quantity.symbol.code() ) != _quantities.end() ) {
            if(_quantities[ quantity.symbol.code() ].symbol == quantity.symbol)
                _quantities[ quantity.symbol.code() ] += quantity;
        }
        else _quantities[ quantity.symbol.code() ] = quantity;

        // symcodes (+1)
        _symcodes[ quantity.symbol.code() ] += 1;
    }

    // codes (+1)
    for ( const name code : codes ) {
        _codes[ code ] += 1;
    }

    // executors (+1)
    _executors[ executor ] += 1;

    // profit (add)
    if ( _profits.find( profit.symbol.code() ) != _profits.end() ) {
        if(_profits[ profit.symbol.code() ].symbol == profit.symbol)
            _profits[ profit.symbol.code() ] += profit;
    }
    else _profits[ profit.symbol.code() ] = profit;

    // save table
    if ( itr == _trades.end() ) {
        _trades.emplace( get_self(), [&]( auto & row ) {
            row.contract = contract;
            row.last_modified = current_time_point();
            row.transactions = 1;
            row.borrow = _borrow;
            row.quantities = _quantities;
            row.codes = _codes;
            row.symcodes = _symcodes;
            row.executors = _executors;
            row.profits = _profits;
        });
    } else {
        _trades.modify( itr, same_payer, [&]( auto & row ) {
            row.last_modified = current_time_point();
            row.transactions += 1;
            row.borrow = _borrow;
            row.quantities = _quantities;
            row.codes = _codes;
            row.symcodes = _symcodes;
            row.executors = _executors;
            row.profits = _profits;
        });
    }
}


[[eosio::action]]
void sx::stats::gatewaylog(const asset in, const asset out, const vector<name> exchanges, const asset savings,const asset fee ) {
    const name contract = get_first_receiver();
    require_auth( contract );

    check( contract.suffix() != "sx"_n, "contract must be *.sx account");

    sx::stats::gateway _gateway( get_self(), get_self().value );
    auto itr = _gateway.find( contract.value );

    // initial variables
    map<symbol_code, pair<uint64_t, asset>> _ins;
    map<symbol_code, pair<uint64_t, asset>> _outs;
    map<name, uint64_t>                     _exchanges;
    map<symbol_code, asset>                 _savings;
    map<symbol_code, asset>                 _fees;

    // append current stats if exists
    if ( itr != _gateway.end() ) {
        _ins = itr->ins;
        _outs = itr->outs;
        _exchanges = itr->exchanges;
        _savings = itr->savings;
        _fees = itr->fees;
    }

    if(_ins.count(in.symbol.code())) {
        _ins[in.symbol.code()].first += 1;
         if(_ins[in.symbol.code()].second.symbol == in.symbol) _ins[in.symbol.code()].second += in;  //check for exact symbol to avoid failing on OGX,4 vs OGX,8
    }
    else _ins[in.symbol.code()] = {1, in};

    if(_outs.count(out.symbol.code())) {
        _outs[out.symbol.code()].first += 1;
        if(_outs[out.symbol.code()].second.symbol == out.symbol) _outs[out.symbol.code()].second += out;
    }
    else _outs[out.symbol.code()] = {1, out};

    for(const auto& dex: exchanges) {
        _exchanges[dex] += 1;
    }

    if(_savings.count(savings.symbol.code())){
        if(_savings[ savings.symbol.code() ].symbol == savings.symbol) _savings[ savings.symbol.code() ] += savings;
    }
    else _savings[ savings.symbol.code() ] = savings;

    if(fee.amount) {
        if(_fees.count(fee.symbol.code())) {
            if(_fees[ fee.symbol.code() ].symbol == fee.symbol) _fees[ fee.symbol.code() ] += fee;
        }
        else _fees[ fee.symbol.code() ] = fee;
    }

    // save table
    if ( itr == _gateway.end() ) {
        _gateway.emplace( get_self(), [&]( auto & row ) {
            row.contract = contract;
            row.last_modified = current_time_point();
            row.transactions = 1;
            row.ins = _ins;
            row.outs = _outs;
            row.exchanges = _exchanges;
            row.savings = _savings;
            row.fees = _fees;
        });
    } else {
        _gateway.modify( itr, same_payer, [&]( auto & row ) {
            row.last_modified = current_time_point();
            row.transactions += 1;
            row.ins = _ins;
            row.outs = _outs;
            row.exchanges = _exchanges;
            row.savings = _savings;
            row.fees = _fees;
        });
    }

}

void sx::stats::update_spot_prices( const name contract )
{
    sx::stats::spotprices _spotprices( get_self(), get_self().value );
    auto itr = _spotprices.find( contract.value );
    const symbol_code base = contract == "swap.sx"_n ? symbol_code{"EOS"} : symbol_code{"USDT"};
    const map<symbol_code, double> quotes = get_spot_prices( contract, base );

    // save table
    if ( itr == _spotprices.end() ) {
        _spotprices.emplace(get_self(), [&]( auto & row ) {
            row.contract = contract;
            row.last_modified = current_time_point();
            row.base = base;
            row.quotes = quotes;
        });
    } else {
        _spotprices.modify( itr, same_payer, [&]( auto & row ) {
            row.last_modified = current_time_point();
            row.base = base;
            row.quotes = quotes;
        });
    }
}

bool sx::stats::is_token_exists( const name contract, const symbol_code symcode )
{
    sx::swap::tokens _tokens( contract, contract.value );
    return _tokens.find( symcode.raw() ) != _tokens.end();
}

double sx::stats::get_spot_price( const name contract, const symbol_code base, const symbol_code quote )
{
    if ( !is_token_exists( contract, base ) ) return 0;
    if ( !is_token_exists( contract, quote ) ) return 0;

    const auto [reserve_in, reserve_out] = sx::swap::get_virtual_reserves( contract, base, quote );
    return sx::utils::asset_to_double( reserve_in ) / sx::utils::asset_to_double( reserve_out );
}

map<symbol_code, double> sx::stats::get_spot_prices( const name contract, const symbol_code base )
{
    sx::swap::tokens _tokens( contract, contract.value );
    map<symbol_code, double> spot_prices;

    for ( const auto token : _tokens ) {
        const symbol_code quote = token.sym.code();
        spot_prices[ quote ] = get_spot_price( contract, base, quote );
    }
    return spot_prices;
}