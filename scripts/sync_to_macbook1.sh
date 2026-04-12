#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "${SCRIPT_DIR}/.." && pwd)"
REMOTE_HOST="${REMOTE_HOST:-macbook1}"
REMOTE_DIR="${REMOTE_DIR:-~/unreal-flight-sim}"

case "$REMOTE_DIR" in
  "~")
    REMOTE_DIR_MKDIR='$HOME'
    ;;
  "~/"*)
    REMOTE_DIR_MKDIR="\$HOME/${REMOTE_DIR#~/}"
    ;;
  *)
    REMOTE_DIR_MKDIR="$REMOTE_DIR"
    ;;
esac

ssh -o BatchMode=yes "$REMOTE_HOST" "mkdir -p \"$REMOTE_DIR_MKDIR\""

rsync -az --delete \
  -e "ssh -o BatchMode=yes" \
  --exclude '.git/' \
  --exclude 'Binaries/' \
  --exclude 'DerivedDataCache/' \
  --exclude 'Intermediate/' \
  --exclude 'Saved/' \
  --exclude 'out/' \
  "$PROJECT_ROOT"/ "$REMOTE_HOST":"$REMOTE_DIR"/

echo "synced project to $REMOTE_HOST:$REMOTE_DIR"
