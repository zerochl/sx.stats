#include <sx.swap/swap.sx.hpp>
#include <sx.utils/utils.hpp>

#include "stats.sx.hpp"

void sx::stats::on_swaplog( const name buyer,
                            const asset amount_in,
                            const asset amount_out,
                            const asset fee )
{
    // require_auth( get_self() );
    const name contract = get_first_receiver();
    if ( contract.suffix() != "sx"_n) return;

    update_volume( contract, vector<asset>{ amount_in, amount_out }, fee );
    update_spot_prices( contract );
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

void sx::stats::on_flashlog( const name receiver,
                             const asset borrow,
                             const asset fee,
                             const asset reserve )
{
    // require_auth( get_self() );
    const name contract = get_first_receiver();
    if ( contract.suffix() != "sx"_n) return;

    update_flash( contract, borrow, fee, reserve );
}

void sx::stats::update_flash( const name contract, const asset borrow, const asset fee, const asset reserve )
{
    sx::stats::flash _flash( get_self(), get_self().value );
    auto itr = _flash.find( contract.value );

    // initial variables
    map<symbol_code, asset> borrows;
    map<symbol_code, asset> fees;
    map<symbol_code, asset> reserves;

    // append current stats if exists
    if ( itr != _flash.end() ) {
        borrows = itr->borrow;
        fees = itr->fees;
        reserves = itr->reserves;
    }

    // reserves (replace with current)
    reserves[ reserve.symbol.code() ] = reserve;

    // fees (add)
    if ( fees.find( fee.symbol.code() ) != fees.end() ) fees[ fee.symbol.code() ] += fee;
    else fees[ fee.symbol.code() ] = fee;

    // borrow (add)
    if ( borrows.find( borrow.symbol.code() ) != borrows.end() ) borrows[ borrow.symbol.code() ] += borrow;
    else borrows[ borrow.symbol.code() ] = borrow;

    // save table
    if ( itr == _flash.end() ) {
        _flash.emplace( get_self(), [&]( auto & row ) {
            row.contract = contract;
            row.last_modified = current_time_point();
            row.transactions = 1;
            row.borrow = borrows;
            row.fees = fees;
            row.reserves = reserves;
        });
    } else {
        _flash.modify( itr, same_payer, [&]( auto & row ) {
            row.last_modified = current_time_point();
            row.transactions += 1;
            row.borrow = borrows;
            row.fees = fees;
            row.reserves = reserves;
        });
    }
}

void sx::stats::update_spot_prices( const name contract )
{
    sx::stats::spotprices _spotprices( get_self(), get_self().value );
    auto itr = _spotprices.find( contract.value );
    const symbol_code base = symbol_code{"USDT"};
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

    const auto [reserve_in, reserve_out] = sx::swap::get_reserves( contract, base, quote );
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