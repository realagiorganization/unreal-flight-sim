# Build And Release

## Prerequisites

- Unreal Engine source checkout or install available locally
- `UNREAL_ENGINE_ROOT` pointing at that engine
- `git lfs install --local` already run in this repo

## Local editor workflow

```bash
export UNREAL_ENGINE_ROOT="$HOME/volumes/unreal-engine-sources"
./scripts/generate_project_files.sh
./scripts/build_with_source_engine.sh
./scripts/open_editor_with_source_engine.sh
```

## Local packaging

```bash
export UNREAL_ENGINE_ROOT="$HOME/volumes/unreal-engine-sources"
TARGET_PLATFORM=Linux TARGET_CONFIG=Shipping \
  ./scripts/package_with_source_engine.sh
```

Default outputs:

- package directory: `out/releases/UnrealFlightSim-<Platform>-<Config>/`
- zip archive: `out/releases/UnrealFlightSim-<Platform>-<Config>.zip`

## GitHub Actions packaging

`.github/workflows/package-release.yml` expects a self-hosted runner with labels:

- `self-hosted`
- `linux`
- `unreal`

Required runner/repo configuration:

- environment variable or Actions variable `UNREAL_ENGINE_ROOT`
- Unreal dependencies already installed on the runner
- enough disk for cook/package output

## Release flow

1. Push changes to `main`
2. Verify hygiene passes
3. Create a tag like `v0.1.0`
4. Push the tag
5. The self-hosted package workflow builds, archives, uploads artifacts, and
   creates or updates the GitHub release for that tag
