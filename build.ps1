if (Test-Path ".\build\codify.exe") {
    Move-Item ".\build\codify.exe" ".\codify.exe" -Force
}
if (Test-Path ".\build\codify") {
    Move-Item ".\build\codify" ".\codify" -Force
}
Remove-Item -Recurse -Force ".\build"
New-Item -ItemType Directory -Path ".\build"
Set-Location ".\build"
cmake -G "MinGW Makefiles" ..
cmake --build . --config Release
Move-Item ".\codify.exe" "..\codify.exe" -Force
if (Test-Path ".\codify") {
    Move-Item ".\codify" "..\codify" -Force
}