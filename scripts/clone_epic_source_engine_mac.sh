#!/usr/bin/env bash
set -euo pipefail

ENGINE_BRANCH="${ENGINE_BRANCH:-5.5}"
ENGINE_DEST="${ENGINE_DEST:-$HOME/UnrealEngine}"
ENGINE_REMOTE="${ENGINE_REMOTE:-git@github.com:EpicGames/UnrealEngine.git}"

if [[ -e "$ENGINE_DEST" && ! -d "$ENGINE_DEST/.git" ]]; then
  BACKUP_DEST="${ENGINE_DEST}.codex-backup-$(date +%Y%m%d-%H%M%S)"
  mv "$ENGINE_DEST" "$BACKUP_DEST"
  printf 'moved existing %s to %s\n' "$ENGINE_DEST" "$BACKUP_DEST"
fi

if [[ ! -d "$ENGINE_DEST/.git" ]]; then
  git clone --depth 1 --single-branch --branch "$ENGINE_BRANCH" "$ENGINE_REMOTE" "$ENGINE_DEST"
else
  git -C "$ENGINE_DEST" fetch --depth 1 origin "$ENGINE_BRANCH"
  git -C "$ENGINE_DEST" checkout -B "$ENGINE_BRANCH" FETCH_HEAD
fi

printf 'source engine checkout ready at %s (%s)\n' "$ENGINE_DEST" "$ENGINE_BRANCH"
