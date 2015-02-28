#!/bin/bash -xe
EPDDIR="$1"

mkdir -p epd
cp -f "$EPDDIR"/*.epd epd/
