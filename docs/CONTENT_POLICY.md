# Content Policy

The repository permits a small committed Unreal `Content/` footprint, but only
under strict hygiene rules.

## Allowed

- `Content/**/*.uasset`
- `Content/**/*.umap`
- Source code, config, scripts, docs, and workflow files

## Required

- `.uasset` and `.umap` must be Git LFS tracked
- Unreal binary assets must live under `Content/`
- Repo hygiene must pass via `./scripts/verify_repo_hygiene.sh`

## Forbidden

- `Binaries/`, `Intermediate/`, `Saved/`, `DerivedDataCache/`, `.vs/`
- Packaged build outputs, archives, and cooked content
- Unreal asset binaries outside `Content/`

## Intent

This keeps the project publishable and reviewable while still allowing the first
playable level, materials, and other minimal in-editor assets to live in git.
