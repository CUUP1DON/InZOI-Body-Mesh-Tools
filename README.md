# InZOI Body Mesh Tools
> [!IMPORTANT]
> This plugin was made using AI IDEs like cursor and copilot in visual studio! Most I have done is just test to make sure everything is operational. Thank claude sonnet & opus 4.6 for the code <3

This is an Unreal Engine plugin that serializes B1 morph target data as an Asset User Data object and adds anim graph stubs for custom ABPs, allowing them to survive cooking and packaging so that sliders work with custom bodies and ABPs in InZOI.

It's been a while since I've modded for InZOI so I don't know if people really need this plugin or not but it was useful to me and I was like 'why not share it?'. So, here we are. 

This is a resource for modders who want to make custom bodies and keep sliders & morphs intact. Not a game mod.

## Requirements

5.4.4 (Needs Unversioned Properties Fixed Rebuilt by FrancisLouiss, GITHUB ACC REQUIRED)

Blender v4.5 (addon might work with newer version)


## Download
To download the plugin, go to [Releases](https://github.com/CUUP1DON/InZOI-Body-Mesh-Tools/releases) and click the .zip for your version of Unreal.
Download The required body JSONs too. They will be kept up to date.


## Known Issues
Feedback and feature requests are very much wanted! I built this plugin with cursor and copilot in vs studio so if there are any issues, please, please, please!!!! let me know!!!! Below are a list of currently known issues I plan to fix.

* none at the moment...

## Things To Note Before Use

Unreal may freeze during transfer. The tool loads and parses the entire JSON file (including all morph data and 10,000+ per-vertex deltas), so Unreal can stop responding for a while. Let it finish and do not force-quit the editor. 

Opening the skeletal mesh afterward may also freeze the editor while Unreal loads the mesh and the new asset user data. Wait for it to finish and do not close Unreal. Expect the same when closing the window.

## Instructions
- [Nexus Mods page](https://www.nexusmods.com/inzoi/mods/1286)
