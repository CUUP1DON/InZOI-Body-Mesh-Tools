# InZOI Body Mesh Tools
> [!IMPORTANT]
> This plugin was created using the help of claude.ai.

## Requirements
Each plugin is made for a specific Unreal version: 5.4.4, 5.6.0 and Inzoi ModKit 5.4.4

> [!IMPORTANT]
>On April 23rd, InZOI moves to Unreal Engine 5.6. This release includes a 5.6 build of the plugin. After that date, the 5.4.4 plugin build will no longer be updated or maintained.

## Things To Note Before Use

Unreal may freeze during transfer. The tool loads and parses the entire JSON file (including all morph data and 10,000+ per-vertex deltas), so Unreal can stop responding for a while. Let it finish and do not force-quit the editor. Opening the skeletal mesh afterward may also freeze the editor while Unreal loads the mesh and the new asset user data. Wait for it to finish and do not close Unreal. Expect the same when closing the window.

## Download
To download the plugin, go to [Releases](https://github.com/CUUP1DON/InZOI-Body-Mesh-Tools/releases) and click the .zip for your version of Unreal.
Download The required body JSONs too. They will be kept up to date.





## Install
https://github.com/user-attachments/assets/187e71f1-afdf-4c29-bebd-d4342c2bac3e
1. Unzip and drop the BlueClientCompat into your project's plugins folder
2. Open your project
3. Go to Edit > Plugins
4. Search for 'BlueClient'
5. Enable 'BlueClient Compatibility' if it is not already

## Where To Find
https://github.com/user-attachments/assets/b79c87f1-0fb5-42f1-ae29-3f82008320ae

Tools > inZOI Body Mesh Tools

## How To Use
### Transfer Section: 


https://github.com/user-attachments/assets/0ac7b5c9-87cd-4277-b7cf-5558cd9a167c


This section is used to transfer the B1MorphTarget & assign the ABP_BonePostProcess user data to your custom body mesh.
1. Find the JSON file of the body you are modifying using the 'Browse InZOI Body JSON'
2. Next to 'Target Skeletal Mesh', click the drop down and find your mesh
3. Next to 'Post Process Anim Blueprint', click the drop down and find your dummy asset which should be named ABP_BonePostProcess
4. Click Transfer

If all goes well, your custom body mesh will have the ABP and B1Morph data:
<img width="1904" height="997" alt="Screenshot (19)" src="https://github.com/user-attachments/assets/3ee9ad68-8ced-4711-b35e-76e6c0ce25a0" />


### Materials Section:


https://github.com/user-attachments/assets/8859bfcb-307b-42fa-b511-23a86b70cfd7



This section is used to assign the correct material & slot name to all 32 material slots without having to do it manually.
1.  Next to 'Target Skeletal Mesh', make sure your skeletal mesh is selected
2.  Next to 'Uniform Section Material', click the drop down and find your material (i.e. MI_Female_Body_001)
3.  Next to 'Uniform Material Slot Name' type in the material slot name (i.e. MI_Female_Body_001)

If all goes well, your custom body mesh will have the assigned material and slot names:
<img width="1911" height="1002" alt="Screenshot (20)" src="https://github.com/user-attachments/assets/dda4d7eb-0d7b-4b86-ab2b-0295fd5aa047" />


Click 'Save Packages' to save these changes.
