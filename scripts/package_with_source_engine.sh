#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "${SCRIPT_DIR}/.." && pwd)"
UNREAL_ENGINE_ROOT="${UNREAL_ENGINE_ROOT:-$HOME/volumes/unreal-engine-sources}"
UPROJECT_PATH="${UPROJECT_PATH:-$PROJECT_ROOT/UnrealFlightSim.uproject}"
TARGET_NAME="${TARGET_NAME:-UnrealFlightSim}"
TARGET_PLATFORM="${TARGET_PLATFORM:-Linux}"
TARGET_CONFIG="${TARGET_CONFIG:-Shipping}"
ARCHIVE_ROOT="${ARCHIVE_ROOT:-$PROJECT_ROOT/out/releases}"
PACKAGE_NAME="${PACKAGE_NAME:-${TARGET_NAME}-${TARGET_PLATFORM}-${TARGET_CONFIG}}"
ARCHIVE_DIR="${ARCHIVE_DIR:-$ARCHIVE_ROOT/$PACKAGE_NAME}"
ZIP_PATH="${ZIP_PATH:-$ARCHIVE_ROOT/${PACKAGE_NAME}.zip}"
RUN_UAT_SCRIPT="$UNREAL_ENGINE_ROOT/Engine/Build/BatchFiles/RunUAT.sh"
RUN_UAT_EXTRA_ARGS="${RUN_UAT_EXTRA_ARGS:-}"
CREATE_ZIP="${CREATE_ZIP:-1}"

if [[ ! -x "$RUN_UAT_SCRIPT" ]]; then
  echo "missing RunUAT script: $RUN_UAT_SCRIPT" >&2
  exit 1
fi

"$SCRIPT_DIR/verify_repo_hygiene.sh"
mkdir -p "$ARCHIVE_ROOT"
rm -rf "$ARCHIVE_DIR" "$ZIP_PATH"

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
  -pak \
  -prereqs \
  -utf8output \
  "${extra_args[@]}"

if [[ "$CREATE_ZIP" == "1" ]]; then
  parent_dir="$(dirname "$ARCHIVE_DIR")"
  base_name="$(basename "$ARCHIVE_DIR")"
  (
    cd "$parent_dir"
    zip -r "$ZIP_PATH" "$base_name"
  )
  echo "wrote release archive: $ZIP_PATH"
fi
