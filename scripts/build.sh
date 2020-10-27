#!/bin/bash

eosio-cpp stats.sx.cpp -I ../
cleos set contract stats.sx . stats.sx.wasm stats.sx.abi