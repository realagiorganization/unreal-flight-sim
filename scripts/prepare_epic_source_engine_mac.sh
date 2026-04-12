#!/usr/bin/env bash
set -euo pipefail

ENGINE_ROOT="${ENGINE_ROOT:-$HOME/UnrealEngine}"

if [[ ! -x "$ENGINE_ROOT/Setup.sh" || ! -x "$ENGINE_ROOT/GenerateProjectFiles.sh" ]]; then
  echo "expected Setup.sh and GenerateProjectFiles.sh under $ENGINE_ROOT" >&2
  exit 1
fi

cd "$ENGINE_ROOT"
./Setup.sh
./GenerateProjectFiles.sh

printf 'prepared source engine at %s\n' "$ENGINE_ROOT"
