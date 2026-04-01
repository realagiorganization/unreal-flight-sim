# Unreal Flight Sim

`unreal-flight-sim` is a source-only Unreal C++ prototype focused on a compact
flight loop: spawn a simple airplane, keep it airborne, and fly with keyboard
input immediately after opening the project in Unreal.

## What is included

- A source-only `.uproject`
- A custom `AFlightSimPawn` assembled from built-in engine primitive meshes
- A lightweight `UFlightSimMovementComponent` with throttle, pitch, roll, and yaw
- Project config for desktop defaults and basic input bindings
- Helper scripts for generating project files, building, opening the editor, and
  verifying the repo remains free of generated Unreal outputs

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

### Root-repo harness integration

If this repo lives inside the `/home/standard` root checkout, the existing
Unreal harness there can prepare the engine volume and deferred source clone:

```bash
cd /home/standard
make unreal-source-harness
make unreal-source-checkout APPLY=1
```

Then return here and run the scripts above.

## Current limitations

- No binary map or art assets are committed yet
- No in-editor verification was possible in this environment because Unreal is
  not installed locally
- The prototype is intentionally code-first so the repo can stay clean and easy
  to publish
