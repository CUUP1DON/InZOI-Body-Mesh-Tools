# InZOI Body Mesh Tools

An Unreal Engine 5.4 plugin that serializes B1 morph target data as an Asset User Data object and adds anim graph stubs for custom Animation Blueprints, allowing them to survive cooking and packaging so that body sliders work with custom bodies and ABPs in inZOI.

---

## Features

- **`B1MorphTargetUserData`** — Stores B1 morph target delta data (position + tangent per vertex, per LOD) directly on a Skeletal Mesh asset as Asset User Data, so the data survives cooking and pak packaging.
- **`UB1BonePostProcessAnimInstance`** — Stub ABP parent class whose UClass path (`/Script/BlueClient.B1BonePostProcessAnimInstance`) matches inZOI's internal class. Set this as the parent when creating a custom Animation Blueprint so the game recognises it as a valid post-process anim instance.
- **`FB1AnimNode_BoneGroupTransform`** — Anim graph node stub matching inZOI's runtime node. Place it between your local-space pose and any AnimDynamics nodes. At runtime the game's binary applies the per-bone group transforms that drive body sliders/morphs.
- **Editor transfer tool** — Slate UI panel (`Window → BlueClient Compat Transfer`) for importing morph data from FModel JSON or a Blender addon export, assigning a custom ABP, and uniformizing material slots on the target Skeletal Mesh.

---

## Recommended AnimGraph Wiring

For a custom ABP that supports both body morphs **and** physics:

```
[Linked Input Pose]
      │  (local space)
[B1 Bone Group Transform]   ← this node
      │  (local space, morphs applied)
[Convert Local to Component Space]
      │  (component space)
[AnimDynamics ×7]
      │  (component space)
[Convert Component to Local Space]
      │
[Output Pose]
```

---

## Installation

1. Copy the plugin folder into your project's `Plugins/` directory (or the engine's `Plugins/` directory).
2. Regenerate project files and rebuild.
3. Enable **BlueClient Compatibility (with Jiggle)** in the Plugins panel if it is not enabled automatically.

> **Note:** The compiled module is named `BlueClient` so its UClass paths match what inZOI expects at runtime (`/Script/BlueClient.*`).

---

## Usage

### Importing morph data

1. Open the transfer tool via **Window → BlueClient Compat Transfer**.
2. Set the **Target Skeletal Mesh** and optionally a **Post-Process Anim Blueprint**.
3. Provide the path to an **FModel JSON** export *or* a **Blender addon JSON** export and click **Import / Transfer**.
4. Save the asset. The `B1MorphTargetUserData` object is now embedded in the mesh and will survive packaging.

### Creating a compatible ABP

1. **File → New Animation Blueprint**.
2. Set **Parent Class** to `B1BonePostProcessAnimInstance`.
3. In the AnimGraph, wire `B1 Bone Group Transform` between your input pose and any downstream component-space nodes.
4. Assign the ABP to your Skeletal Mesh's **Post Process Anim Blueprint** slot.

---

## Changelog

### v2.0
- Plugin renamed to **BlueClient Compatibility (with Jiggle)**.
- Added `B1BonePostProcessAnimInstance` and `B1AnimNode_BoneGroupTransform` stubs so jiggle-physics ABPs can also carry inZOI slider/morph data.

### v0.0.2
- Added ability to import custom body JSON from the Blender addon for accurate deltas for muscle morphs.

### v0.0.1
- Initial release.

---

## Permissions

- **Modification:** Allowed with credit.
- **Asset use:** Allowed with credit.
- **Re-upload / conversion to other games:** Not allowed.
- **Use in mods sold for money:** Not allowed.
- **Use in mods earning Donation Points:** Allowed.

---

## Links

- [Nexus Mods page](https://www.nexusmods.com/inzoi/mods/1286)
