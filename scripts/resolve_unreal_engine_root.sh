#!/usr/bin/env bash
set -euo pipefail

has_unreal_root() {
  local candidate="$1"
  [[ -n "$candidate" ]] || return 1
  [[ -x "$candidate/Engine/Build/BatchFiles/RunUAT.sh" ]] || return 1
}

print_if_valid() {
  local candidate="$1"
  if has_unreal_root "$candidate"; then
    printf '%s\n' "$candidate"
    exit 0
  fi
}

print_if_valid "${UNREAL_ENGINE_ROOT:-}"
print_if_valid "$HOME/volumes/unreal-engine-sources"
print_if_valid "$HOME/UnrealEngine"

if [[ "$(uname -s)" == "Darwin" ]]; then
  while IFS= read -r candidate; do
    print_if_valid "$candidate"
  done < <(find "/Users/Shared/Epic Games" -maxdepth 1 -type d -name 'UE_*' 2>/dev/null | sort)
fi

cat >&2 <<'EOF'
unable to resolve Unreal Engine root
searched:
  $UNREAL_ENGINE_ROOT
  $HOME/volumes/unreal-engine-sources
  $HOME/UnrealEngine
  /Users/Shared/Epic Games/UE_* (macOS)
expected:
  <root>/Engine/Build/BatchFiles/RunUAT.sh
EOF
exit 1
