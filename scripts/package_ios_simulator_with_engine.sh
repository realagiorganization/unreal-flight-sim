#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "${SCRIPT_DIR}/.." && pwd)"
UNREAL_ENGINE_ROOT="${UNREAL_ENGINE_ROOT:-$("$SCRIPT_DIR/resolve_unreal_engine_root.sh")}"
UPROJECT_PATH="${UPROJECT_PATH:-$PROJECT_ROOT/UnrealFlightSim.uproject}"
TARGET_NAME="${TARGET_NAME:-UnrealFlightSim}"
TARGET_PLATFORM="${TARGET_PLATFORM:-IOSSimulator}"
TARGET_CONFIG="${TARGET_CONFIG:-Development}"
ARCHIVE_ROOT="${ARCHIVE_ROOT:-$PROJECT_ROOT/out/releases}"
PACKAGE_NAME="${PACKAGE_NAME:-${TARGET_NAME}-${TARGET_PLATFORM}-${TARGET_CONFIG}}"
ARCHIVE_DIR="${ARCHIVE_DIR:-$ARCHIVE_ROOT/$PACKAGE_NAME}"
RUN_UAT_SCRIPT="$UNREAL_ENGINE_ROOT/Engine/Build/BatchFiles/RunUAT.sh"
RUN_UAT_EXTRA_ARGS="${RUN_UAT_EXTRA_ARGS:-}"

if [[ ! -x "$RUN_UAT_SCRIPT" ]]; then
  echo "missing RunUAT script: $RUN_UAT_SCRIPT" >&2
  exit 1
fi

"$SCRIPT_DIR/verify_repo_hygiene.sh"
mkdir -p "$ARCHIVE_ROOT"
rm -rf "$ARCHIVE_DIR"

read -r -a extra_args <<<"$RUN_UAT_EXTRA_ARGS"

"$RUN_UAT_SCRIPT" BuildCookRun \
  -project="$UPROJECT_PATH" \
  -noP4 \
  -target="$TARGET_NAME" \
  -platform="$TARGET_PLATFORM" \
  -clientconfig="$TARGET_CONFIG" \
  -build \
  -cook \
  -stage \
  -package \
  -archive \
  -archivedirectory="$ARCHIVE_DIR" \
  -skipcodesign \
  -utf8output \
  "${extra_args[@]}"

echo "wrote simulator package: $ARCHIVE_DIR"
