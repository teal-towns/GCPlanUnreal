## Setup

- Copy the `Source/ConditionalExample` folder and adjust for your content.
	- Create a new folder for your project / assets and add this path to `settings.json`
	- Add content (meshes and materials) to `Content/Conditional` and update `contentMeshes.json` to reference them.
	- Copy `CCDefines.h` to `Source/GCPlan/CCDefines.h` and modify accordingly.
	- Landscape (this currently has to be manually set in the project editor - TODO: fix this so it is auto-updated via code and can live in the conditional project folder so changing `settings.json` will be all that is necessary.)
		- add `image-terrain.jpg` and a 16-bit grayscale `image-heightmap.png` to the root of the `Source/Conditional` folder
		- add `image-terrain-[size].jpg` to the `Content/Conditional/Landscape` (sub)folder
			- This should update the landscape, but if it does not, to get the Landscape in Unreal Editor to actually reflect the changes, you must re-assign the (same) material to ALL proxy partions (select them all and change all at once).
		- Re-import the heightmap in the Unreal Editor Landscape mode: 1. Select the Landscape in the Scene Outliner, 2. Choose the Landscape Mode from the (top left) dropdown, 3. Manage -> Import -> check Heightmap File -> select file -> Import
			- Note: if you get an error on file selection, try moving the heightmap file to its own directory on your computer first so there are no other files in the same directory as it since Unreal sometimes errors otherwise.

### Mac
- Build from Unreal Editor with the hotkey (button is missing..). Default is Command + Option + Shift + P which is annoying - go to viewport (top left 3 bars icon) -> Advanced Settings then search for `compile` and find the Keyboard Shortcuts section `Recompile Game Code` entry and set it to something simpler like Cmd+B.
- VSCode has `'CoreMinimal.h' file not found` error so was not able to build from VSCode
- Visual Studio can not be used because Visual Studio for Mac does not support C++
    - https://forums.unrealengine.com/t/mac-generate-visual-studio-project-files/467048
    - https://visualstudio.microsoft.com/vs/mac/

## Common Tasks

### Editor and Game Mode
- Use `UFUNCTION` to expose a button in editor to trigger code and abstract / reduce use of `GameInstance` and other game / runtime specific code to write code that works in BOTH the editor and at runtime.
	- Editor mode allows for quicker debugging.
	- Editor mode is required for sequencer / movie making and HLODs / performance.

### Assets (3D Models, Textures)
- Texture file format: PNG for transparency, JPG otherwise (for reduced file size)
	- https://forums.unrealengine.com/t/tga-vs-png-for-textures/24211/22
	- TGA is common and good, but much larger file size; can convert to PNG or JPG, e.g. https://products.aspose.app/imaging/image-resize

### Modeling
- Use `ModelingStructsActor` as inputs (update / add to structs to add more input options / customizations).
- Hierarchy
	- Use `LayoutModelActor` for building rooms or lands / nature (using models generated from ModelingStructActor)
	- Use `ModelingActor` for building 3D models (using / combining existing primitive meshes generated from ProceduralModelActor)
		- (e.g. combine 4 cubes as table legs and 1 cube as a table top to make a table).
	- Use `ProceduralModelActor` to generate new meshes (if need new "lego" building blocks / primitives)
		- Generate small pieces with proper triangles then rotate / move (e.g. for rounding corners, generate ONE corner then duplicate and rotate / move it for all other corners; otherwise code is more complicated and triangles are likely to be off for some of the sides).
- After meshes are generated, for each object merge meshes / actors (select actors then `Actor -> Merge Actors`) then save with `Actor -> Convert "..." to Static Mesh` to save something in the scene as a reusable mesh (which can then be inputs, e.g. to `ModelingActor` or `LayoutModelActor`). In general use this for unique (smaller) models (e.g. procedural mesh or reusuable components) rather than full rooms or large layouts (which likely will be one of a kind and can be generated in code easily). One of each unique component should be able to build an entire city.
- `ModelingActor` and `LayoutModelActor` are similar, but 3d models from `ModelingActor` will likely be saved as a new static mesh and re-used whereas rooms will likely always be re-built on the fly and will be unique / have many more possible combinations (and will be large file sizes with duplicated meshes so we don't want to save them).
- Units: meters (will be converted to Unreal centimeters at the end).
- Pivot: 0,0,0 would be centered for X & Y and bottom for Z (aligned to ground). So builds (expands) out from center for X & Y but up from bottom for Z. ALL models should use this save pivot system for consistency.
- For consistent orientation / rotation, use Unreal XYZ coordinate system so assume positive X is forward (front), positive Y is right. This means in the editor you would be looking at the back of the object, so build as if you are looking at it from the back (so right side of couch is right from behind, left from in front looking at it head on). E.g. for a couch with scale (0.1, 0.2, 0.2), the sides would be:
	- back: scale = FVector(scale.X, size.Y, size.Z); location = FVector(-1 * size.X / 2 + scale.X / 2, 0, 0);
	- right: scale = FVector(size.X, scale.Y, size.Z); location = FVector(0, size.Y / 2 - scale.Y / 2, 0);
	- left: scale = FVector(size.X, scale.Y, size.Z); location = FVector(0, -1 * size.Y / 2 + scale.Y / 2, 0);
	- bottom: scale = FVector(size.X, size.Y, scale.Z); location = FVector(0, 0, 0);

### Include Third Party Code
- https://georgy.dev/posts/third-party-integration/
	- Have to update `GCPlan.Build.cs` with both `PublicAdditionalLibraries` and `PublicIncludePaths`

### (Git) Workflow
- `git pull origin main` to get up to date code, then `git checkout -b [branch]` to create a new branch for your work.
- [Do the work]
- `git status -s` to see file changes. It is not uncommon for files you did not touch to be edited in the process; in general revert those changes with `git checkout -- [filename]` optionally with wildcards for pattern matching: https://stackoverflow.com/questions/15160978/is-there-a-way-to-use-wildcards-with-git-checkout
	- Ensure no large file changes that are not vital: `git status --porcelain | awk "{print $2}" | xargs ls -hl` (then search for `G ` and `M ` for gigabytes and megabytes files)
	- Check & do above steps (for any mesh, texture or materials additions or changes).
- After confirmed the work is good, `git commit -am 'SUMMARY-OF-CHANGES'` (then `git checkout main` `git pull origin main` `git checkout [branch]` `git rebase main` to pull in any current changes since you started work) then `git push origin [branch]` then go to github.com and open a Pull Request and request a review / tell a team member.
	- Once code is reviewed, changed (if needed) and approved, merge the Pull Request on Github, do a `git pull origin main` locally to get your changes and delete the branch on github.com and locally.


## Conditional Content
`Conditional` folders are git ignored for project specific content.
- Add references to `contentMeshes.json` to use them. Code will load with fallbacks if these assets are not present.
- Content is broken up into Git repos, roughly by: 1. project / related content (e.g. nature biomes), 2. file size (low quality (low poly, small textures) vs high poly (high poly, large textures)).


## Coding

- C++ has no reflection so can not do:
	- nested arrays without a struct in-between..
		- https://forums.unrealengine.com/t/tarray-of-tarray-support-request/357202
	- JSON (de)serialization without Unreal `FJsonObjectConverter` which requires UStruct
		- e.g. a simple json stringify in other languages is not possible in C++..
	- named / optional function arguments in any order.. Instead need to pass ALL arguments in order, meaning if we want to pass the 5th named argument but want to leave the 3rd and 4th as default, we can not; we have to set 3rd and 4th also..
- UStruct requires an empty constructor or will give compile errors
	- https://forums.unrealengine.com/t/how-to-write-constructor-for-custom-struct/341749
- FRotator is NOT x,y,z it is y, z, x (x = roll, y = pitch, z = yaw)


- Styleguide / Coding Conventions: https://docs.unrealengine.com/4.26/en-US/ProductionPipelines/DevelopmentSetup/CodingStandard/

- The Unreal C++ documentation seems to just be (auto-generated?) lists of SOME of the source code (I was not able to find all methods and the Search on their site returned 0 results) with no / few code examples, so I found it pretty useless. Instead I opened the engine source code in a code editor (VSCode, Sublime) at your Unreal Editor install location (e.g. `~/Unreal/UE_5.1/Engine/Source`) and searched it to try to find methods and variables for functions.

### C++ Syntax

- https://en.cppreference.com/w/cpp/language/type
- https://en.cppreference.com/w/c/language/struct_initialization
- https://blog.jamesbrooks.net/posts/struct-to-json-serialization/
- https://unrealcommunity.wiki/logging-lgpidy6i


## Sequencer

- Any actors that are moved must NOT be "spatially loaded" https://forums.unrealengine.com/t/5-1-level-sequence-cannot-play-in-a-world-partition-level/694103/4 or they will not work in play mode (but will in Sequencer Editor)
