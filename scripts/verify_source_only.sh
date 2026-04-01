#!/usr/bin/env bash
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_ROOT="$(cd "${SCRIPT_DIR}/.." && pwd)"

cd "$PROJECT_ROOT"

mapfile -t bad_dirs < <(find . -type d \( \
  -name Binaries -o \
  -name DerivedDataCache -o \
  -name Intermediate -o \
  -name Saved -o \
  -name .vs \
\) -print | sort)

mapfile -t bad_files < <(find . -type f \( \
  -iname '*.uasset' -o \
  -iname '*.umap' -o \
  -iname '*.pak' -o \
  -iname '*.exe' -o \
  -iname '*.dll' -o \
  -iname '*.so' -o \
  -iname '*.dylib' \
\) -print | sort)

if (( ${#bad_dirs[@]} > 0 )); then
  printf 'generated directories are not allowed:\n' >&2
  printf '  %s\n' "${bad_dirs[@]}" >&2
  exit 1
fi

if (( ${#bad_files[@]} > 0 )); then
  printf 'binary Unreal outputs are not allowed:\n' >&2
  printf '  %s\n' "${bad_files[@]}" >&2
  exit 1
fi

echo "source-only verification passed"
