## Setup

### Mac
- Build from Unreal Editor with the hotkey (button is missing..). Default is Command + Option + Shift + P which is annoying - go to viewport (top left 3 bars icon) -> Advanced Settings then search for `compile` and find the Keyboard Shortcuts section `Recompile Game Code` entry and set it to something simpler like Cmd+B.
- VSCode has `'CoreMinimal.h' file not found` error so was not able to build from VSCode
- Visual Studio can not be used because Visual Studio for Mac does not support C++
    - https://forums.unrealengine.com/t/mac-generate-visual-studio-project-files/467048
    - https://visualstudio.microsoft.com/vs/mac/

## Common Tasks

### Assets (3D Models, Textures)
- Texture file format: PNG for transparency, JPG otherwise (for reduced file size)
	- https://forums.unrealengine.com/t/tga-vs-png-for-textures/24211/22
	- TGA is common and good, but much larger file size; can convert to PNG or JPG, e.g. https://products.aspose.app/imaging/image-resize

## Coding

- Styleguide / Coding Conventions: https://docs.unrealengine.com/4.26/en-US/ProductionPipelines/DevelopmentSetup/CodingStandard/

- The Unreal C++ documentation seems to just be (auto-generated?) lists of SOME of the source code (I was not able to find all methods and the Search on their site returned 0 results) with no / few code examples, so I found it pretty useless. Instead I opened the engine source code in a code editor (VSCode, Sublime) at your Unreal Editor install location (e.g. `~/Unreal/UE_5.1/Engine/Source`) and searched it to try to find methods and variables for functions.

### C++ Syntax

- https://en.cppreference.com/w/cpp/language/type
- https://en.cppreference.com/w/c/language/struct_initialization
- https://blog.jamesbrooks.net/posts/struct-to-json-serialization/
- https://unrealcommunity.wiki/logging-lgpidy6i
