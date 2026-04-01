#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "${SCRIPT_DIR}/.." && pwd)"
UNREAL_ENGINE_ROOT="${UNREAL_ENGINE_ROOT:-$HOME/volumes/unreal-engine-sources}"
UPROJECT_PATH="${UPROJECT_PATH:-$PROJECT_ROOT/UnrealFlightSim.uproject}"
TARGET_NAME="${TARGET_NAME:-UnrealFlightSimEditor}"
TARGET_PLATFORM="${TARGET_PLATFORM:-Linux}"
TARGET_CONFIG="${TARGET_CONFIG:-Development}"
BUILD_SCRIPT="$UNREAL_ENGINE_ROOT/Engine/Build/BatchFiles/Linux/Build.sh"

if [[ ! -x "$BUILD_SCRIPT" ]]; then
  echo "missing Unreal build script: $BUILD_SCRIPT" >&2
  exit 1
fi

"$BUILD_SCRIPT" "$TARGET_NAME" "$TARGET_PLATFORM" "$TARGET_CONFIG" "$UPROJECT_PATH"
