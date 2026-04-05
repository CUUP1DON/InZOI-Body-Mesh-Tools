# InZOI Body Mesh Tools
> [!IMPORTANT]
> This plugin was created using the help of claude.ai.

## Requirements
Each plugin is made for a specific Unreal version: Unreal Engine 5.4.4, Unreal Engine 5.6.0 and Inzoi ModKit 5.4.4

> [!IMPORTANT]
>On April 9, InZOI moves to Unreal Engine 5.6. This release includes a 5.6 build of the plugin. After that date, the 5.4.4 plugin build will no longer be updated or maintained.

## Things To Note Before Use
Unreal may freeze during transfer. The tool loads and parses the entire JSON file (including all morph data and 10,000+ per-vertex deltas), so Unreal can stop responding for a while. Let it finish and do not force-quit the editor. Opening the skeletal mesh afterward may also freeze the editor while Unreal loads the mesh and the new asset user data. Wait for it to finish and do not close Unreal. Expect the same when closing the window.

## Download
To download the plugin, go to [Releases](https://github.com/CUUP1DON/InZOI-Body-Mesh-Tools/releases) and click the .zip for your version of Unreal.
Download The required body JSONs too. They will be kept up to date.

## Install
1. Unzip and drop the BlueClientCompat into your project's plugins folder
2. Open your project
3. Go to Edit > Plugins
4. Search for 'BlueClient'
5. Enable 'BlueClient Compatibility' if it is not already

## Where To Find
Tools > inZOI Body Mesh Tools

## How To Use
### Transfer Section: 
This section is used to transfer the B1MorphTarget & assign the ABP_BonePostProcess user data to your custom body mesh.
1. Find the JSON file of the body you are modifying using the 'Browse InZOI Body JSON'
2. Next to 'Target Skeletal Mesh', click the drop down and find your mesh (you can also drags & drop it)
3. Next to 'Post Process Anim Blueprint', click the drop down and find your dummy asset which should be named ABP_BonePostProcess (you can also drags & drop it)
4. Click Transfer

### Materials Section:
This section is used to assign the correct material & slot name to all 32 material slots without having to do it manually.
1.  Next to 'Uniform Section Material', click the drop down and find your material, i.e. MI_Female_Body_001 (you can also drags & drop it)
2.  Next to 'Uniform Material Slot Name' type in the material slot name, i.e. MI_Female_Body_001

Click 'Save Packages' to save these changes.
