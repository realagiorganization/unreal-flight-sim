# Unreal Flight Sim

`unreal-flight-sim` is a source-first Unreal C++ prototype focused on a compact
flight loop: spawn a simple airplane, keep it airborne, and then grow it into a
playable simulator with a small committed `Content/` footprint.

## Active development track

The current product direction is bush flying.

See [docs/BUSH_FLYING_ROADMAP.md](docs/BUSH_FLYING_ROADMAP.md) for the concrete
milestones, technical priorities, and content scope.

## What is included

- A source Unreal `.uproject`
- A custom `AFlightSimPawn` assembled from built-in engine primitive meshes
- A lightweight `UFlightSimMovementComponent` with throttle, pitch, roll, and yaw
- Project config for desktop defaults and basic input bindings
- Helper scripts for generating project files, building, packaging, opening the
  editor, and verifying repo hygiene
- Git LFS policy for committed Unreal map and asset binaries
- GitHub Actions scaffolding for hygiene checks and self-hosted packaging

## Repository policy

This repo is no longer strict source-only. It now allows a minimal committed
`Content/` tree with these rules:

- `.uasset` and `.umap` files are allowed only under `Content/`
- Unreal binary assets must be Git LFS tracked via [.gitattributes](.gitattributes)
- Generated directories like `Binaries/`, `Intermediate/`, `Saved/`, and
  `DerivedDataCache/` remain forbidden
- Packaged builds stay in local `out/` directories and are never committed

See [docs/CONTENT_POLICY.md](docs/CONTENT_POLICY.md) for the exact guardrails.

## Controls

- `W` / `S`: throttle up / down
- `Up` / `Down`: pitch
- `A` / `D`: roll
- `Q` / `E`: yaw
- Mouse: orbit the chase camera
- `R`: reset aircraft state

## Local build flow

This repository does not vendor Unreal Engine. Point it at an existing launcher
install or a source checkout.

### Source engine checkout

```bash
export UNREAL_ENGINE_ROOT="$HOME/volumes/unreal-engine-sources"
./scripts/generate_project_files.sh
./scripts/build_with_source_engine.sh
./scripts/open_editor_with_source_engine.sh
```

### Local packaging

```bash
export UNREAL_ENGINE_ROOT="$HOME/volumes/unreal-engine-sources"
./scripts/package_with_source_engine.sh
```

The default package output lands in `out/releases/` and is zipped for release
upload.

### Root-repo harness integration

If this repo lives inside the `/home/standard` root checkout, the existing
Unreal harness there can prepare the engine volume and deferred source clone:

```bash
cd /home/standard
make unreal-source-harness
make unreal-source-checkout APPLY=1
```

Then return here and run the scripts above.

## CI and release path

- `./scripts/verify_repo_hygiene.sh` enforces the content policy locally and in CI
- `.github/workflows/repo-hygiene.yml` runs the policy check on every push/PR
- `.github/workflows/package-release.yml` is wired for a self-hosted Unreal runner
  and creates packaged artifacts plus tag releases

## Current limitations

- No real `Content/` assets or levels are committed yet
- No in-editor verification was possible in this environment because Unreal is
  not installed locally
- Packaging automation is scaffolded but untested here without a local engine

See [docs/BUILD_AND_RELEASE.md](docs/BUILD_AND_RELEASE.md) for the exact release
workflow.
