#!/bin/bash

set -euo pipefail

TARGET=app
BOARD=nucleo_u575zi_q

BUILD_DIR="build/$BOARD/$TARGET"

west build -b "$BOARD" -d "$BUILD_DIR" "$TARGET"
west flash -d "$BUILD_DIR"
