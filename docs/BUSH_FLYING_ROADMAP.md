# Bush Flying Roadmap

This branch turns `unreal-flight-sim` into a short-haul bush-flying game rather
than a general-purpose flight sandbox.

## Product identity

The player flies a rugged light aircraft through difficult terrain, rough
weather, and short improvised airstrips. Success comes from energy management,
terrain reading, visual navigation, and careful landings rather than instrument
complexity or combat.

## Pillars

1. Terrain matters.
Low valleys, ridgelines, tree cover, and narrow strips should shape every
flight.

2. Weather matters.
Wind, turbulence, visibility, and changing light should alter approach and
route choices.

3. Landings matter.
Short strips, uneven surfaces, and load weight should make arrival quality the
core scoring axis.

4. Missions matter.
Cargo, medevac, survey, and mail routes should provide structure without turning
this into a spreadsheet economy sim.

## First playable slice

The first meaningful bush-flying vertical slice should include:

- one handcrafted valley region
- one mountain ridge crossing
- one primary airstrip and one rough backup strip
- one aircraft tuned for STOL-style handling
- one mission type: cargo delivery with weather and landing grading
- one failure loop: stall, overspeed-on-approach, runway overrun, terrain impact

## Milestones

### Milestone 1: Flight feel over terrain

Goal: flying close to terrain already feels different from a generic prototype.

Work:
- tune low-speed handling and lift loss near stall
- add simple wind and gust parameters to the movement model
- add altitude-above-ground awareness
- create chase camera behavior that emphasizes terrain proximity
- expose tuning values for bush-plane handling rather than fast-cruise behavior

Definition of done:
- the same aircraft feels harder to manage in valleys and on approach
- steep low-speed turns are visibly risky
- the player can recover from gusts but not ignore them

### Milestone 2: Rough-strip landing loop

Goal: landings become the core skill test.

Work:
- implement landing evaluation: touchdown speed, sink rate, runway remaining,
  centerline deviation
- add strip surface metadata: grass, dirt, gravel, roughness
- add reset/retry flow specific to approach practice
- build one short-strip pattern mission

Definition of done:
- a player can clearly tell the difference between a clean landing and a bad one
- the game can score and restart approach attempts quickly

### Milestone 3: Mission framework

Goal: bush flying gains structure.

Work:
- mission contracts for cargo, medevac, and survey
- payload weight modifier affecting climb and landing roll
- route briefing screen with weather, distance, and strip notes
- debrief screen with payout/grade and incident summary

Definition of done:
- there is at least one repeatable mission loop with progression value

### Milestone 4: Weather and world pressure

Goal: route planning becomes meaningful.

Work:
- localized wind strength by zone
- fog, rain, and cloud-base presets
- time-of-day variants affecting visibility and terrain reading
- optional emergency diversion when a strip becomes unsafe

Definition of done:
- the same route feels materially different under at least three weather states

### Milestone 5: Content expansion

Goal: the project becomes a small game rather than a test map.

Work:
- second region with a different terrain profile
- more strips with different runway lengths and surfaces
- second aircraft archetype
- stronger progression and unlock pacing

## Code implications

The current codebase should evolve in these directions first:

- `UFlightSimMovementComponent`: wind, gusts, stall margin, weight/load inputs,
  and ground-proximity queries
- `AFlightSimPawn`: approach assist telemetry, bush-plane camera tuning, and
  rough-strip reset checkpoints
- mission system: separate route contracts from raw flight movement code
- scoring/debrief layer: landing quality and mission completion should be first-
  class systems, not ad hoc UI text

## Content implications

The first committed `Content/` additions should be narrow and intentional:

- one test map
- one landscape material set
- one strip kit for rough runways and simple signage
- one minimal UI asset pack for brief/debrief screens

Do not start with a huge library of props, aircraft, or regions.
The first win is one memorable valley route that proves the identity.

## Recommended next implementation order

1. Add low-speed/bush-plane tuning hooks to the movement component.
2. Add wind/gust support with debug visualization.
3. Commit the first playable map under `Content/`.
4. Add landing evaluation and debrief output.
5. Add one cargo mission contract.
