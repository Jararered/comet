set windows-shell := ["powershell.exe", "-NoProfile", "-Command"]

REPO_ROOT := `git rev-parse --show-toplevel`

build_dir := "build"
target := "Comet-Editor"

# Build in Debug mode and launch the editor from its output directory.
# Unix single-config generators emit build/Comet-Editor/Comet-Editor;
# Windows multi-config emits build/Comet-Editor/Debug/Comet-Editor.exe.

[unix]
debug:
    cmake -S "{{REPO_ROOT}}" -B "{{REPO_ROOT}}/{{build_dir}}" -DCMAKE_BUILD_TYPE=Debug
    cmake --build "{{REPO_ROOT}}/{{build_dir}}" --config Debug
    cd "{{REPO_ROOT}}/{{build_dir}}/{{target}}" && "./{{target}}"

[windows]
debug:
    cmake -S "{{REPO_ROOT}}" -B "{{REPO_ROOT}}/{{build_dir}}" -DCMAKE_BUILD_TYPE=Debug
    cmake --build "{{REPO_ROOT}}/{{build_dir}}" --config Debug
    Push-Location "{{REPO_ROOT}}/{{build_dir}}/{{target}}/Debug"; & "./{{target}}.exe"; $exitCode = $LASTEXITCODE; Pop-Location; exit $exitCode
