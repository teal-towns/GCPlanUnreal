## Setup

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
- Use `ModelingStructsActor` as inputs (update / add to structs to add more input options)
- Use `ProceduralModelActor` to generate new meshes (if need new "lego" building blocks / primitives)
- Use `ModelingActor` to combine existing meshes (and optionally genereate new meshes via ProceduralModel) (e.g. combine 4 cubes as table legs and 1 cube as a table top to make a table).
- After meshes are generated, use `Actor -> Convert "..." to Static Mesh` to save something in the scene as a reusable mesh (which can then be inputs to `ModelingActor`).

### Include Third Party Code
- https://georgy.dev/posts/third-party-integration/
	- Have to update `GCPlan.Build.cs` with both `PublicAdditionalLibraries` and `PublicIncludePaths`

### (Git) Workflow
- `git pull origin main` to get up to date code, then `git checkout -b [branch]` to create a new branch for your work.
- [Do the work]
- `git status -s` to see file changes. It is not uncommon for files you did not touch to be edited in the process; in general revert those changes with `git checkout -- [filename]` optionally with wildcards for pattern matching: https://stackoverflow.com/questions/15160978/is-there-a-way-to-use-wildcards-with-git-checkout
	- Ensure no large file changes that are not vital: `git status --porcelain | awk "{print $2}" | xargs ls -hl` (then search for `G ` and `M ` for gigabytes and megabytes files)
	- Check & do above steps (for any mesh, texture or materials additions or changes).
- After confirmed the work is good, `git commit -am 'SUMMARY-OF-CHANGES'` then `git push origin [branch]` then go to github.com and open a Pull Request and request a review / tell a team member.
	- Once code is reviewed, changed (if needed) and approved, merge the Pull Request on Github, do a `git pull origin main` locally to get your changes and delete the branch on github.com and locally.


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
