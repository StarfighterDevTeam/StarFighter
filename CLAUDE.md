# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## What this is

**StarFighter** is a 2D space-shooter/roguelite ("Beta") written in C++ with SFML 2.6.1 (graphics/audio). It has a hub ("stellar map"), procedurally-composed scenes, factions, a grid-based inventory/equipment system, enemy pools/phases, dialogs, and a shop/upgrade economy. Primary target is Windows / Visual Studio 2022 (v143 toolset), x86. There is partial macOS support (an Xcode project, `ResourcePath.mm`, `#ifdef __APPLE__` path resolution), though it isn't actively maintained.

There is **no engine/game split** — this is a single monolithic project (`StarFighter`). An earlier iteration of this codebase (see git history) had a reusable `StarEngine` with `SE*` base classes overridden by a `Proto`/`StarFighter` game; that split no longer exists. Don't assume `SE*`-prefixed types, factory functions (`CreateGame`, `CreateShip`, etc.), or a separate engine project — everything lives directly under `Unstable/StarFighter/StarFighter/`.

There is no test suite, linter, or CI in this repo.

## Repo layout

```
Libraries/                         Vendored third-party deps (SFML-2.6.1, SFGUI (unused, vendored only), VCRedist)
Unstable/StarFighter/
  StarFighter.sln                  The one solution to open
  Tools/                           Packaging script + excluded-extensions list
  Documentation/                   Design docs, mockups, moodboards, storyboards (not code)
  StarFighter/                     Project root (.vcxproj + .xcodeproj live here)
    StarFighter/                   All C++ source (.h/.cpp), flat — no subfolders per module
    Includes/                      A couple of header-only third-party utilities (e.g. SimpleCollision.hpp)
    Assets/                        Runtime assets (2D, Fonts, Music, Sounds, Icons, Scripts)
    Saves/                         Save files
```

## Building

Open `Unstable/StarFighter/StarFighter.sln` in Visual Studio 2022+ and build `Release|Win32` or `Debug|Win32` (only x86 is configured). This is the only project — there is nothing else to build first.

The build expects SFML as a **static** lib: `SFML_STATIC` is defined and headers/libs come from `Libraries/SFML-2.6.1/` (referenced from the project as `..\..\..\Libraries\SFML-2.6.1\include` / `\lib`). Linked libs: `sfml-graphics-s`, `sfml-window-s`, `sfml-audio-s`, `sfml-system-s` (plus `flac`, `freetype`, `ogg`, `openal32`, `vorbis*`, `glu32`, `opengl32`, `winmm`).

## Packaging a release (Windows)

From `Unstable/StarFighter/Tools/`, after building `Release|Win32`:

- `PackageWin32Release.bat` — copies `Assets/`, `Saves/`, the built `.exe`, and the VC++ redistributable (`Libraries/VCRedist/x86`) into a `StarFighter/` distributable folder, excluding extensions listed in `excludedextensions.txt` (`.ods`, `.pdb`, `.ico`).

## Architecture

### Entry point and main loop

`StarFighter.cpp` → `main()`: creates the save directory, loads `PrefsManager` (config/prefs singleton), opens the SFML `RenderWindow`, constructs the global `Game`, initializes a `Player`, then pushes a `Gameloop` state onto a `GameManager` state stack. The loop itself (resolution switching, frame timing, delegating `Update`/`Draw` to the current state) lives in `StarFighter.cpp`.

### State machine (`GameManager` / `GameState`)

`GameState` is an abstract base (`Initialize`, `Update`, `Draw`, `Release`) with a `GameManager*` back-pointer. `GameManager` owns a `std::stack<GameState*>` and exposes `PushState` / `SetState` / `PopState` / `GetCurrentState`. `Gameloop` (in `Gameloop.h/.cpp`) is the (currently only) concrete state — it drives scene loading, the boss/hub/scrolling state machine (`GameloopStateMachine`: `LOADING`, `SCROLLING`, `LAST_SCREEN`, `BOSS_FIGHT`, `HUB_ROAMING`), CSV-driven content loading (`LoadAllScenes`, `LoadAllEnemies`, `LoadAllUpgrades`, etc.), and panel creation (`CreateSFPanel`/`DestroySFPanel`).

### Global singleton

`Game* CurrentGame` (defined in `StarFighter.cpp`, declared `extern` where needed, e.g. `Gameloop.h`) is the single global holding runtime state — scene contents, rectangles/texts/panels/text-pops to draw, SFX/music banks, the player ship. Code reads/writes through `(*CurrentGame)...` rather than dependency injection; expect this pattern when tracing behavior.

### Object model

`GameObject` (`GameObject.h/.cpp`) extends the vendored `AnimatedSprite` (`AnimatedSprite.hpp`, third-party, in `StarFighter/`) and is the base for everything drawable/updatable in the scene: `LayerType` (draw-order layers) and `GameObjectType` (`BackgroundObject`, `PortalObject`, `PlayerShip`, `EnemyObject`, `LootObject`, etc.) are both declared directly in `GameObject.h` — there is no separate `Enums.h`. `Ship` (`Ship.h/.cpp`) extends `GameObject` and adds the action/input state machine: `PlayerInputStates` (`Input_Release`/`Input_Tap`/`Input_Hold`) driven by `InputGuy` (static input-polling helpers: `getDirections`, `isFiring`, `isBraking`, etc.) and `PlayerActions`.

### Panels (custom UI, not ImGui)

There is no ImGui/in-game-tooling layer in this codebase. UI is a family of `SF*Panel` classes (`SFPanel` base, plus `SFHUDPanel`, `SFInventoryPanel`, `SFMapPanel`, `SFTradePanel`, `SFUpgradesPanel`, `SFDialogPanel`, `SFItemStatsPanel`, `SFMenuPanel`) built on SFML primitives (`SFText`, `SFTextPop`). `SFPanelTypes` (declared in `GameObject.h`) enumerates panel kinds; `Gameloop::CreateSFPanel`/`DestroySFPanel` manage which panel is active.

### World generation and content systems

- `Scene` (`Scene.h/.cpp`) — a loaded level/room, CSV-configured (`ScenesData` fields: links in 4 directions, background, music, hub flag, etc.).
- `Grid` (`Grid.h/.cpp`) — grid-based equipment/inventory slots (`SlotFeedbackStates`, `CursorFeedbackStates`) used by the trade/upgrades panels.
- `GeometryPattern` (`GeometryPattern.h/.cpp`) and `IPatternHandler.h` — procedural enemy/formation patterns.
- `EnemyGenerator` / `EnemyPool` / `Phase` / `Bot` — enemy spawning, pooling, and scripted phases (boss fights etc.), organized by `FactionType` (`Vanguard`, `Royale`, `Corsair`, `Ancient`, `Swarm`).
- `Weapon` / `Ammo` / `Upgrade` / `Shop` / `Loot` / `Portal` / `Dialog` — the rest of the RPG-lite economy and scene-transition/dialog systems.

### Assets and data

- Runtime assets live under `Unstable/StarFighter/StarFighter/Assets/` (`2D`, `Fonts`, `Music`, `Sounds`, `Icons`, `Scripts`). Asset paths are always resolved via `makePath()` (`Globals.cpp`) — never hardcode an `Assets/` prefix; pass the sub-path to `makePath`. `makePath`/`createSavesDirectory`/`getSavesPath` branch on `__APPLE__` for bundle-relative paths vs. Windows-relative paths.
- Content is CSV-driven: `Assets/Scripts/Generated/*.csv` (`Ship.csv`, `Scenes.csv`, `Enemies.csv`, `EnemyPhases.csv`, `Equipment.csv`, `Weapons.csv`, `Ammo.csv`, `Upgrades.csv`, `Bot.csv`, `Dialogs.csv`, `FX.csv`) are parsed by `FileLoaderUtils::FileLoader` (`FileLoadUtils.h`) into `vector<vector<string>>` tables; lines/cells starting with `#` are comments. These CSVs are generated from spreadsheets (see `Documentation/*.xlsx`, `Macro_ExportCSV.txt`).
- Save files live in `Saves/` (`KnownScenes.txt`, `MoneyAndHealth.txt`, `ShopUpgrades.txt`, `Upgrades.txt` on Windows; app-support dir on macOS).

### Tunable constants

Gameplay/window constants are `#define`s in `Globals.h` (resolution, scene size, music/SFX volumes and fade times, hub-exit timings) and `Assets.h` (asset filenames/dimensions, color constants). Ship physics constants (`SHIP_ACCELERATION_X`, `SHIP_MAX_SPEED_X`, `SHIP_ARMOR`, `SHIP_SHIELD`, etc.) are in `Ship.h`. Prefer adjusting these over magic numbers in logic.

## Conventions

- Hungarian-ish member prefixes: `m_` for members, `NBVAL_`/`NB_` as the count sentinel at the end of every enum (used for array sizing and modulo cycling).
- Enums live next to the system they describe (`GameObject.h`, `Ship.h`, `Game.h`, `Scene.h`, `Grid.h`, `SFPanel.h`, ...) rather than in one shared header — check the relevant class's header first when looking for a constant/enum.
- All source is flat inside `Unstable/StarFighter/StarFighter/StarFighter/` — there's no per-feature folder structure; group by filename (e.g. all `SF*Panel` files are the UI layer).
