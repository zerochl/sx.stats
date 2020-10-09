#!/bin/bash

eosio-cpp stats.sx.cpp -I include
cleos set contract stats.sx . stats.sx.wasm stats.sx.abi