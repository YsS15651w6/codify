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
if (Test-Path ".\codify.exe") {
    if (Test-Path "..\codify.exe") { Remove-Item "..\codify.exe" -Force }
    Move-Item ".\codify.exe" "..\codify.exe"
}
if (Test-Path ".\codify") {
    if (Test-Path "..\codify") { Remove-Item "..\codify" -Force }
    Move-Item ".\codify" "..\codify"
}