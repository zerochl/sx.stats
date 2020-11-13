# `stats.sx`

> SX Statistics

## Table of Content

- [TABLE `volume`](#table-volume)
- [TABLE `spotprices`](#table-spotprices)
- [TABLE `flash`](#table-flash)
- [TABLE `trades`](#table-trades)

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

## TABLE `trades`

- `{name} contract` - (primary key) contract name
- `{time_point_sec} last_modified` - last modified timestamp
- `{uint64_t} transactions` - total amount of transactions
- `{map<symbol_code, asset>} quantities` - total quantity traded
- `{map<name, uint64_t>} codes` - total transactions per contract codes
- `{map<symbol_code, uint64_t>} symbcodes` - total transactions per symbol code used
- `{map<name, uint64_t>} executors` - total transactions per executor
- `{map<symbol_code, asset>} profits` - total profits

### example

```json
{
    "contract": "basic.sx",
    "last_modified": "2020-06-03T00:00:00",
    "transactions": 640,
    "borrow": [
        {"key": "EOS", "value": "5030.3050 EOS"},
        {"key": "USDT", "value": "400.0100 USDT"}
    ],
    "quantities": [
        {"key": "EOS", "value": "5030.3050 EOS"},
        {"key": "USDT", "value": "400.0100 USDT"}
    ],
    "codes": [
        {"key": "swap.defi", "value": 512},
        {"key": "swap.sx", "value": 100}
    ],
    "symcodes": [
        {"key": "EOS", "value": 610},
        {"key": "USDT", "value": 30}
    ],
    "executors": [
        {"key": "myaccount", "value": 80},
        {"key": "miner.sx", "value": 200}
    ],
    "profits": [
        {"key": "EOS", "value": "50.3050 EOS"},
        {"key": "USDT", "value": "4.0100 USDT"}
    ]
}
```