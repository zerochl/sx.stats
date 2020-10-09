#include "stats.sx.hpp"

void sx::stats::on_log( const name buyer,
                        const asset quantity,
                        const asset rate,
                        const asset fee )
{
    require_auth( get_self() );

    update_volume( vector<asset>{ quantity, rate }, fee );
}

void sx::stats::update_volume( const vector<asset> volumes, const asset fee )
{
    sx::stats::volume _volume( get_self(), get_self().value );
    auto volume = _volume.get_or_default();

    // daily timestamp (at 0:00 UTC)
    const time_point_sec timestamp = time_point_sec( (current_time_point().sec_since_epoch() / 86400) * 86400 );

    // reset volume if new daily period
    if ( timestamp > volume.timestamp ) {
        volume.volume = map<symbol_code, asset>{};
        volume.fees = map<symbol_code, asset>{};
        volume.timestamp = timestamp;
    }

    // volume
    for ( const asset quantity : volumes ) {
        if ( volume.volume.find( quantity.symbol.code() ) != volume.volume.end() ) volume.volume[ quantity.symbol.code() ] += quantity;
        else volume.volume[ quantity.symbol.code() ] = quantity;
    }

    // fees
    if ( volume.fees.find( fee.symbol.code() ) != volume.fees.end() ) volume.fees[ fee.symbol.code() ] += fee;
    else volume.fees[ fee.symbol.code() ] = fee;

    _volume.set( volume, get_self() );
}
