# CLAUDE.md

This file provides guidance to Claude Code (claude.ai/code) when working with code in this repository.

## What this is

A 2D space-shooter game ("Proto") and the reusable engine it runs on ("StarEngine"), written in C++ using SFML 2.6.1 (graphics/audio) and Dear ImGui (in-game tooling/UI). Primary target is Windows / Visual Studio 2015 (v140 toolset), x86. There is partial macOS support guarded by `#ifdef __APPLE__` (asset/save path resolution), but no macOS build files exist in the repo.

## Building

Open the solution in Visual Studio 2015+ and build `Release|Win32` or `Debug|Win32` (only x86 is configured):

- `Proto/Proto.sln` — the game; references `StarEngine.vcxproj` as a dependency, so building this builds the engine too. **This is the one you normally build.**
- `StarEngine/StarEngine.sln` — engine on its own (rarely needed directly).

The build expects SFML as a **static** lib: `SFML_STATIC` is defined and headers/libs come from the vendored `StarEngine/SFML-2.6.1/`. Include paths (set in `Proto.vcxproj`) are: `..\StarEngine`, the vendored SFML include, both vendored ImGui dirs (`imgui-docking`, `imgui-sfml/imgui-sfml-2.6.x`), and `.\EngineConfig\`.

There is no test suite, linter, or CI in this repo.

## Packaging a release (Windows)

From `Tools/`, after building `Release|Win32`:

- `PackageProto.bat` — wraps `PackageWin32Release.bat Proto`
- `PackageStarFighter.bat` — wraps `PackageWin32Release.bat StarFighter`

`PackageWin32Release.bat` copies `Assets/`, `Saves/`, the built `.exe`, `openal32.dll`, and the VS2015 redistributable (`redist-vs2015/`) into a distributable folder, excluding source/build artifacts listed in `excludedextensions.txt`.

## Architecture

The codebase is a deliberate **engine / game split**. `StarEngine/` defines abstract `SE*` base classes plus the main loop; `Proto/` provides concrete subclasses. The wiring between them is the key thing to understand:

### Engine ↔ game contract (factory functions)

`StarEngine.cpp` declares four factory functions it does **not** implement:

```cpp
SEInGameState* CreateInGameState();
SEInputs*      CreateInputs();
SEShip*        CreateShip();
SEGame*        CreateGame();
```

The game (`Proto/Main.cpp`) implements them, returning its concrete `Game`, `InGameState`, `Inputs`, `Ship`. `StarEngine::Init()` calls these to build the world. This is how the engine stays game-agnostic while running game-specific code. To start a new game on the engine, you implement these four functions and subclass the corresponding `SE*` types.

`Main.cpp` is tiny: `engine.Init(title, icon)` → `engine.Run()` (the loop) → `engine.Shut()`.

### Global singletons

Two extern globals are shared everywhere instead of being passed around:

- `CurrentGame` (`SEGame*`) — the live game/scene; defined in `StarEngine.cpp`.
- `InputsManager` (`SEInputs*`) — input state.

Code reads/writes game state through `(*CurrentGame)...` directly. When tracing behavior, expect these globals rather than dependency injection.

### Main loop (`StarEngine::Run`)

Single 60-FPS loop over an SFML window: poll events (forwarded to ImGui) → handle global keys (Escape quits) → handle resolution switching (`WindowResolutions` enum; recreating the window loses settings, so framerate/icon/title are re-applied) → poll player actions (mute, pause) → update and draw the current `SEInGameState`.

### SEGame — the scene/world container

`SEGame` owns essentially all runtime state: the layered scene (`m_sceneGameObjectsLayered[LayerType]` and `m_sceneGameObjectsTyped[GameObjectType]`), HUD/feedback elements, fonts, SFX/music banks, the camera `View`, and CSV-loaded config. Core methods: `addToScene(obj, layer, type)`, `updateScene`, `drawScene`, `colisionChecksV2`, and garbage collection via `collectGarbage`/`cleanGarbage` (objects flag themselves with `m_GarbageMe`). Closest-object and typed-query helpers (`GetClosestObject`, `GetSceneGameObjectsTyped`) are here too.

### Object model

`GameObject` (extends `AnimatedSprite`) is the base for everything drawable/updatable. `SEShip` extends `GameObject` and adds the input/action system; `Ship` (game side) overrides `UpdateInputStates()` and `update()`. Ships hold an action state machine: `m_inputs_states[PlayerActions]` of type `PlayerInputStates` (`Input_Release`/`Input_Tap`/`Input_Hold`), driven by `GetInputState()` + `UpdateAction()`.

### Enums.h lives in the game, not the engine

`Proto/EngineConfig/Enums.h` defines `LayerType`, `GameObjectType`, `PlayerActions`, `SFX_Bank`, `Music_Bank`, `FontsStyle`, etc. The engine `#include "Enums.h"` and relies on these, but the file is on the **game's** include path (`.\EngineConfig\`). This is intentional: each game customizes its layers/object-types/actions, and the engine compiles against that game's enum set. Editing render order or adding an object type means editing this file.

### Assets and data

- All runtime assets live under `Proto/Assets/` (`2D`, `Fonts`, `Music`, `Sounds`, `Icons`, `Scripts`). Asset paths in code are relative to the `Assets/` folder — always loaded via `makePath()` (`Globals.cpp`), which resolves to `Assets/<path>` on Windows and the app bundle resources on macOS. Don't hardcode `Assets/` prefixes; pass the sub-path to `makePath`.
- Game data is CSV-driven. `SHIP_CSV_FILE` = `Scripts/Generated/Ship.csv` is parsed by `FileLoaderUtils::FileLoader` (in `FileLoadUtils.h`) into `SEGame::m_gameObjectsConfig`. Lines/cells starting with `#` are comments. The `.csv` is generated from the `.ods` spreadsheet in `Assets/Scripts/` (e.g. `Ship.ods`).
- Save files: `Saves/SaveFile.txt` on Windows; `~/Library/Application Support/StarFighter/Saves/` on macOS (see `createSavesDirectory`/`getSavesPath`). Windows save-dir creation is a TODO stub.

### Tunable constants

Gameplay/window constants are `#define`s in `StarEngine/Globals.h` (resolution, music/SFX volumes and fade times, collision settings like `PIXEL_PERFECT_COLLISION`/`GRAZE_DISTANCE`, joystick deadzone) and ship physics in `StarEngine/SEShip.h` (`SHIP_ACCELERATION`, `SHIP_MAX_SPEED`, etc.). Prefer adjusting these over magic numbers in logic.

## Conventions

- Hungarian-ish member prefixes: `m_` for members, `NBVAL_` as the count sentinel at the end of every enum (used for array sizing and modulo cycling).
- The engine deliberately calls into game code through the four factory functions and the `SE*` virtual overrides — when adding a feature, decide whether it's engine-generic (goes in `SExxx`) or game-specific (goes in the `Proto/` subclass).
