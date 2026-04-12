#!/usr/bin/env bash
set -euo pipefail

DOWNLOAD_URL="${EPIC_LAUNCHER_DOWNLOAD_URL:-https://launcher-public-service-prod06.ol.epicgames.com/launcher/api/installer/download/EpicGamesLauncher.dmg}"
WORK_DIR="${WORK_DIR:-$HOME/Downloads}"
DMG_PATH="${DMG_PATH:-$WORK_DIR/EpicGamesLauncher.dmg}"
SYSTEM_DEST="/Applications/Epic Games Launcher.app"
USER_DEST="$HOME/Applications/Epic Games Launcher.app"
MOUNT_DIR=""

cleanup() {
  if [[ -n "$MOUNT_DIR" ]] && mount | grep -F "on $MOUNT_DIR " >/dev/null 2>&1; then
    hdiutil detach "$MOUNT_DIR" -quiet || true
  fi
}

trap cleanup EXIT

mkdir -p "$WORK_DIR"
if [[ ! -s "$DMG_PATH" ]]; then
  curl -L --fail --progress-bar "$DOWNLOAD_URL" -o "$DMG_PATH"
fi

ATTACH_OUTPUT="$(hdiutil attach "$DMG_PATH" -nobrowse -readonly)"
MOUNT_DIR="$(printf '%s\n' "$ATTACH_OUTPUT" | awk 'match($0, /\/Volumes\/.*/) { print substr($0, RSTART); exit }')"

if [[ -z "$MOUNT_DIR" || ! -d "$MOUNT_DIR" ]]; then
  echo "unable to determine mounted DMG path" >&2
  exit 1
fi

APP_SOURCE="$(find "$MOUNT_DIR" -maxdepth 2 -type d -name 'Epic Games Launcher.app' -print -quit)"
if [[ -z "$APP_SOURCE" ]]; then
  echo "unable to find Epic Games Launcher.app inside $MOUNT_DIR" >&2
  exit 1
fi

DEST="$SYSTEM_DEST"
if [[ ! -w "$(dirname "$SYSTEM_DEST")" ]]; then
  mkdir -p "$HOME/Applications"
  DEST="$USER_DEST"
fi

rm -rf "$DEST"
ditto "$APP_SOURCE" "$DEST"
xattr -dr com.apple.quarantine "$DEST" 2>/dev/null || true

printf 'installed Epic Games Launcher to %s\n' "$DEST"
