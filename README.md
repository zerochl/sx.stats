# `stats.sx`

> SX Statistics

## Table of Content

- [TABLE `volume`](#table-volume)

## TABLE `volume`

- `{name} contract` - contract name
- `{time_point_sec} timestamp` - daily periods (86400 seconds)
- `{map<symbol_code, asset>} fees` - total fees collected
- `{map<symbol_code, asset>} volume` - total trading volume of assets

### example

```json
{
    "contract": "swap.sx",
    "timestamp": "2020-06-03T00:00:00",
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
