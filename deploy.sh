#!/usr/bin/env bash

set -e

TC=$(which tonos-cli)

if [[ "$URL" == "" ]]; then
  URL="main.ton.dev"
fi

function compute_address() {
  ${TC} -j genaddr "$1.tvc" --abi "$1.abi" | jq -r '.raw_address'
}

function get_balance() {
  tonos-cli -j --url "$URL" account "$1" | jq -r 'map(.balance)[0]'
}

target="microwave"

factory_address=$(compute_address factory)
target_address=$(compute_address "$target")

factory_balance=$(get_balance "$factory_address")

if ((factory_balance < 900000000)); then
  echo "ERROR: Factory balance is too low. Please replenish the balance."
  echo "Factory address:"
  echo "    ${factory_address}"
  exit 1
fi

target_state_init=$(base64 -w 0 < "$target.tvc")
deploy_args=$(printf "{\"dest\":\"%s\",\"state_init\":\"%s\"}" "$target_address" "$target_state_init")
tonos-cli --url "$URL" deploy factory.tvc "$deploy_args" --abi factory.abi
