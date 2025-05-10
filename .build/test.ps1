
[CmdletBinding(PositionalBinding=$false)]
param (
    [Parameter(ValueFromRemainingArguments=$true)]
    [string[]] $ExtraArgs
)

# Standard boilerplate
Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'
$PSDefaultParameterValues['*:ErrorAction'] = 'Stop'

# Go to repo root
$repoRoot = (Resolve-Path "$PSScriptRoot\..").Path
Push-Location $repoRoot

try {
    cd build
    ctest -C Debug -rerun-failed -output-on-failure .
    if (Test-Path "D:/a/scantailor-deviant/scantailor-deviant/build/Testing/Temporary/LastTest.log") {
        gc "D:/a/scantailor-deviant/scantailor-deviant/build/Testing/Temporary/LastTest.log"
    }
} finally {
    Pop-Location
}
