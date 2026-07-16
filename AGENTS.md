# Repository Guidelines

## Project Structure & Module Organization

- `Comet/source/` contains reusable engine and renderer code, including raylib integration, input, timing, and meshes.
- `Comet-Editor/source/` contains the editor/game executable and application-specific systems such as `World`, player logic, and chunk streaming.
- `Comet-Editor/Resources/` contains runtime assets, including shaders and other resource files.
- `libraries/` contains vendored dependencies such as GLM and ImGui. Avoid modifying these unless updating a dependency is the explicit task.
- `world/` contains world data used by the application; keep generated runtime state out of source control where possible.
- `build/` is a generated CMake tree and should not be edited manually.

## Build, Test, and Development Commands

Configure and build Debug with:

```powershell
cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
cmake --build build --config Debug
```

Build only the editor target with `cmake --build build --config Debug --target Comet-Editor`. Run the configured application with `just debug`; it builds first and launches `Comet-Editor.exe` from its output directory. CMake fetches raylib 6.0 when configuring, so an initial configure requires network access.

## Coding Style & Naming Conventions

Use C++23 and `.clang-format`: four-space indentation, Allman braces, left-aligned pointers, and a 200-column limit. Classes and public methods use PascalCase (for example, `ProcessRequestedChunks`); data members use the existing `m_` prefix. Match nearby ownership patterns around renderer queues, mutexes, and resources. Keep changes focused and format touched C++ files with clang-format when available.

## Testing Guidelines

No automated test target or test directory is currently present. At minimum, build Debug and run the editor to exercise the affected path. Rendering, input, and chunk-streaming changes should be checked in the live application, including startup, world loading, camera movement, and shutdown. Record relevant runtime symptoms or screenshots in the pull request.

## Commit & Pull Request Guidelines

Recent commits use short, imperative summaries such as `Cull chunk requests outside the camera frustum`. Follow that style and keep each commit focused. Pull requests should explain the behavior change, identify affected subsystems, include commands run, and attach screenshots or performance observations for visual or runtime changes. Keep generated artifacts and unrelated formatting churn out of commits.

## Configuration & Runtime Notes

Keep source assets/configuration separate from generated output. Do not commit credentials or machine-specific runtime data. When changing resource paths, verify the editor launches from `build/Comet-Editor/Debug` so relative asset lookup remains valid.
