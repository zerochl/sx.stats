# `stats.sx`

> SX Statistics

## Table of Content

- [TABLE `volume`](#table-volume)
- [TABLE `spotprices`](#table-spotprices)

## TABLE `volume`

- `{name} contract` - (primary key) contract name
- `{time_point_sec} last_modified` - last modified timestamp
- `{uint64_t} transactions` - total amount of transactions
- `{map<symbol_code, asset>} fees` - total fees collected
- `{map<symbol_code, asset>} volume` - total trading volume of assets

### example

```json
{
    "contract": "swap.sx",
    "last_modified": "2020-06-03T00:00:00",
    "transactions": 110,
    "volume": [
        {"key": "EOS", "value": "25.0000 EOS"},
        {"key": "USDT", "value": "100.0000 USDT"}
    ],
    "fees": [
        {"key": "EOS", "value": "0.1250 EOS"},
        {"key": "USDT", "value": "0.5000 USDT"}
    ]
}
```

## TABLE `spotprices`

- `{name} contract` - (primary key) contract name
- `{time_point_sec} last_modified` - last modified timestamp
- `{symbol_code} base` - base symbol code
- `{map<symbol_code, double>} quotes` - quotes prices calculated relative to base

### example

```json
{
    "contract": "swap.sx",
    "last_modified": "2020-07-10T15:17:23",
    "base": "USDT",
    "quotes": [
        {"key": "EOS", "value": 2.6098},
        {"key": "USDT", "value": 1.0000}
    ]
}
```

## TABLE `flash`

- `{name} contract` - (primary key) contract name
- `{time_point_sec} last_modified` - last modified timestamp
- `{uint64_t} transactions` - total amount of transactions
- `{map<symbol_code, asset>} borrow` - total borrowed asset from flash contract
- `{map<symbol_code, asset>} fees` - total fees collected
- `{map<symbol_code, asset>} reserves` - total reserve assets of flash contract

### example

```json
{
    "contract": "flash.sx",
    "last_modified": "2020-06-03T00:00:00",
    "transactions": 110,
    "borrow": [
        {"key": "EOS", "value": "2500.0000 EOS"},
        {"key": "USDT", "value": "100.0000 USDT"}
    ],
    "fees": [
        {"key": "EOS", "value": "0.1250 EOS"},
        {"key": "USDT", "value": "0.0100 USDT"}
    ],
    "reserves": [
        {"key": "EOS", "value": "900.0000 EOS"},
        {"key": "USDT", "value": "200.0000 USDT"}
    ]
}
```