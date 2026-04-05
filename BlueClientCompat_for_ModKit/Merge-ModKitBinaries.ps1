# Merges engine-matched B1MorphCompat DLLs from the official inZOI Mod Kit into this release's
# BlueClientCompat folder (same modules, correct Mod Kit 5.4.4 binary signature).
#
# Usage (PowerShell):
#   .\Merge-ModKitBinaries.ps1 -ModKitPluginRoot "E:\path\to\inZOIModKit\Plugins\B1MorphCompat"
#
# Or set MODKIT_B1MORPH_PLUGIN to that folder and run with no arguments.

param(
    [string] $ModKitPluginRoot = $env:MODKIT_B1MORPH_PLUGIN
)

$ErrorActionPreference = "Stop"

$destRoot = Join-Path $PSScriptRoot "compiled\BlueClientCompat"
if (-not (Test-Path $destRoot)) {
    Write-Error "Destination not found: $destRoot"
}

if ([string]::IsNullOrWhiteSpace($ModKitPluginRoot)) {
    Write-Host @"
No Mod Kit plugin path given.

Set the folder that contains the WORKING Mod Kit plugin (the one named B1MorphCompat), e.g.:
  inZOIModKit\inZOIModKit\Plugins\B1MorphCompat

Then run:
  .\Merge-ModKitBinaries.ps1 -ModKitPluginRoot `"E:\path\to\Plugins\B1MorphCompat`"

Or set environment variable MODKIT_B1MORPH_PLUGIN to that path.
"@
    exit 1
}

$ModKitPluginRoot = (Resolve-Path $ModKitPluginRoot).Path
$srcBin = Join-Path $ModKitPluginRoot "Binaries"
if (-not (Test-Path $srcBin)) {
    Write-Error "Binaries not found under: $ModKitPluginRoot"
}

$dstBin = Join-Path $destRoot "Binaries"
New-Item -ItemType Directory -Path $dstBin -Force | Out-Null

Write-Host "Copying engine binaries from:`n  $srcBin`nTo:`n  $dstBin"
Copy-Item -Path (Join-Path $srcBin "*") -Destination $dstBin -Recurse -Force

Write-Host "Done. Kept BlueClientCompat.uplugin, Resources, and Config in:`n  $destRoot"
Write-Host "Copy the whole 'BlueClientCompat' folder to your Mod Kit project's Plugins folder."
