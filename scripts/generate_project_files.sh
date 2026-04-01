#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "${SCRIPT_DIR}/.." && pwd)"
UNREAL_ENGINE_ROOT="${UNREAL_ENGINE_ROOT:-$HOME/volumes/unreal-engine-sources}"
UPROJECT_PATH="${UPROJECT_PATH:-$PROJECT_ROOT/UnrealFlightSim.uproject}"
GEN_SCRIPT="$UNREAL_ENGINE_ROOT/Engine/Build/BatchFiles/Linux/GenerateProjectFiles.sh"

if [[ ! -x "$GEN_SCRIPT" ]]; then
  echo "missing Unreal generate script: $GEN_SCRIPT" >&2
  exit 1
fi

"$GEN_SCRIPT" -project="$UPROJECT_PATH" -game -engine
