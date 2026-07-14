# Get the repo root directory
set shell := ["powershell.exe", "-NoLogo", "-Command"]

REPO_ROOT := `git rev-parse --show-toplevel`

build_dir := "build"
target := "Comet-Editor"

# Build in 'type' mode (default: debug)
debug:
    cmake -S "{{REPO_ROOT}}" -B "{{REPO_ROOT}}/{{build_dir}}" -DCMAKE_BUILD_TYPE=Debug
    cmake --build "{{REPO_ROOT}}/{{build_dir}}" --config Debug
    Push-Location "{{REPO_ROOT}}/{{build_dir}}/{{target}}/Debug"; & "./{{target}}.exe"; $exitCode = $LASTEXITCODE; Pop-Location; exit $exitCode
