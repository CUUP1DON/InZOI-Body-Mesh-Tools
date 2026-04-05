inZOI Mod Kit — BlueClientCompat (UE 5.4.4)
============================================

Same **plugin folder name** and **.uplugin file name** as the other release variants: **BlueClientCompat**.

The C++ **module** names stay **B1MorphCompat** / **B1MorphCompatEditor** (DLLs:
`UnrealEditor-B1MorphCompat*.dll`).

**Engine descriptor:** `BlueClientCompat.uplugin` declares **EngineVersion 5.4.4** and matches the
official Mod Kit **module loading** (runtime + editor **LoadingPhase: Default**).

Mod Kit vs stock Epic (important)
--------------------------------
The Mod Kit editor is a **custom** Unreal **5.4.4** build. Native **.dll** files must match **that**
editor binary, not a stock Epic UE install.

If Unreal warns that **B1MorphCompat** / **B1MorphCompatEditor** are missing or built with a different
engine version:

1. Read **compiled/BlueClientCompat/MODKIT_ENGINE_MATCH.txt**
2. Run **Merge-ModKitBinaries.ps1** pointing at your working **Plugins\B1MorphCompat** folder from the
   official Mod Kit (it copies **Binaries/** only; your **BlueClientCompat** naming and **Resources**
   stay).

Example:

  cd BlueClientCompat_for_ModKit
  .\Merge-ModKitBinaries.ps1 -ModKitPluginRoot "E:\Program Files\EPIC Games\inZOIModKit\inZOIModKit\Plugins\B1MorphCompat"

Then copy **compiled/BlueClientCompat** into your Mod Kit project’s **Plugins** folder.

**Tools menu / icon:** come from **B1MorphCompatEditor.dll**. DLLs from the official Mod Kit match
the editor; a build from stock Epic may show different menu text until you use Mod Kit–matched
binaries.

Layout
------
  compiled/BlueClientCompat/     — uplugin + Resources + Config + Binaries (Binaries may be Epic-built;
                                   use Merge-ModKitBinaries.ps1 for Mod Kit)

  source/                        — full C++ (B1MorphCompat + B1MorphCompatEditor), for stock UE 5.4.4
                                   **RunUAT BuildPlugin** on a full engine install

Rebuild (developers, stock Epic UE 5.4.4)
-----------------------------------------
Use a **short path** (e.g. **C:\Temp\BCM\BlueClientCompat**) if **MAX_PATH** breaks **BuildPlugin**.

Install (Mod Kit)
-----------------
  <ModKitProject>/Plugins/BlueClientCompat/BlueClientCompat.uplugin
  <ModKitProject>/Plugins/BlueClientCompat/Binaries/Win64/UnrealEditor-B1MorphCompat*.dll
  <ModKitProject>/Plugins/BlueClientCompat/Resources/Icon128.png
