#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "${SCRIPT_DIR}/.." && pwd)"
UNREAL_ENGINE_ROOT="${UNREAL_ENGINE_ROOT:-$HOME/volumes/unreal-engine-sources}"
UPROJECT_PATH="${UPROJECT_PATH:-$PROJECT_ROOT/UnrealFlightSim.uproject}"
EDITOR_BIN="$UNREAL_ENGINE_ROOT/Engine/Binaries/Linux/UnrealEditor"

if [[ ! -x "$EDITOR_BIN" ]]; then
  echo "missing UnrealEditor binary: $EDITOR_BIN" >&2
  exit 1
fi

"$EDITOR_BIN" "$UPROJECT_PATH"
