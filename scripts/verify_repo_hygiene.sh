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

mapfile -t packaged_files < <(find . -type f \( \
  -iname '*.pak' -o \
  -iname '*.exe' -o \
  -iname '*.dll' -o \
  -iname '*.so' -o \
  -iname '*.dylib' \
\) -print | sort)

mapfile -t misplaced_assets < <(find . -type f \( \
  -iname '*.uasset' -o \
  -iname '*.umap' \
\) ! -path './Content/*' -print | sort)

mapfile -t content_assets < <(find ./Content -type f \( \
  -iname '*.uasset' -o \
  -iname '*.umap' \
\) -print 2>/dev/null | sort)

if (( ${#bad_dirs[@]} > 0 )); then
  printf 'generated directories are not allowed:\n' >&2
  printf '  %s\n' "${bad_dirs[@]}" >&2
  exit 1
fi

if (( ${#packaged_files[@]} > 0 )); then
  printf 'packaged or compiled outputs are not allowed:\n' >&2
  printf '  %s\n' "${packaged_files[@]}" >&2
  exit 1
fi

if (( ${#misplaced_assets[@]} > 0 )); then
  printf 'Unreal binary assets must live under Content/:\n' >&2
  printf '  %s\n' "${misplaced_assets[@]}" >&2
  exit 1
fi

for asset in "${content_assets[@]}"; do
  attr_line="$(git check-attr filter -- "$asset")"
  attr_value="${attr_line##*: }"
  if [[ "$attr_value" != "lfs" ]]; then
    printf 'asset is not LFS-tracked: %s\n' "$asset" >&2
    exit 1
  fi
done

echo "repo hygiene verification passed"
