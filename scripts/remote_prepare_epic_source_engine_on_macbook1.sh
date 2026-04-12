#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
REMOTE_HOST="${REMOTE_HOST:-macbook1}"
REMOTE_DIR="${REMOTE_DIR:-~/unreal-flight-sim}"

case "$REMOTE_DIR" in
  "~")
    REMOTE_DIR_CD='$HOME'
    ;;
  "~/"*)
    REMOTE_DIR_CD="\$HOME/${REMOTE_DIR#~/}"
    ;;
  *)
    REMOTE_DIR_CD="$REMOTE_DIR"
    ;;
esac

"$SCRIPT_DIR/sync_to_macbook1.sh"
ssh -o BatchMode=yes "$REMOTE_HOST" "cd \"$REMOTE_DIR_CD\" && chmod +x scripts/prepare_epic_source_engine_mac.sh && ./scripts/prepare_epic_source_engine_mac.sh"
