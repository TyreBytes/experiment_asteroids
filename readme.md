
# Asteroids

Scavenge for resources in this harsh world and speed your way by opponents to rule the wasteland.

Copyright (c) 2025 Tyre Bytes LLC - All Rights Reserved

## Developer Debugging Keys

* `` ` `` or `~` to open the Developer Console and `help` is a command to get started there.
* `1`, `2`, or `3` is used to end a sequence early, or jump directly to a new place.
* `0` will enable or disable "debug mode" which is likely rendering a bunch of extra dev info.
* `9` will force the InterfaceTarget aspect ratio to 4:3 and render safe-zones/layout info if debug is enabled.
* Holding `space` may give timing elements a significant boost to move things along quickly.

## To Build This Project

To get this project building you will need premake5 and it will need to be added to PATH.

This project depends on [TurtleBrains](git@github.com:TyreBytes/turtle_brains.git), [Internal Combusion Engine](git@github.com:TyreBytes/ice.git) and [Track Builder](git@github.com:TyreBytes/track_builder.git) among perhaps dependencies included in the project repository. For setup it is recommended to keep the project and all engine dependencies in a single folder. _Note: This doesn't mean put engine dependencies in the Asteroids project folder, but rather a structure like:_

	tyre_bytes/
		turtle_brains/
		ice/
		track_builder/
		tool_box/
		asteroids/

This structure should have things pretty much "JUST WORKING" otherwise you'll need to modify the `asteroids/build/asteroids.lua` premake file to point to the correct locations in your specific setup.

	git clone git@github.com:TyreBytes/turtle_brains.git
	git clone git@github.com:TyreBytes/ice.git
	git clone git@github.com:TyreBytes/track_builder.git
	git clone git@github.com:TyreBytes/tool_box.git
	git clone git@github.com:TyreBytes/asteroids.git

### All Platforms Need to Create ToolBox SymLink
1. Open CMDPROMT / Terminal and navigate to `asteroids/source`
2. On Windows use `MKLINK /J tool_box ..\..\tool_box\source\tool_box` to create symlink.
3. On Linux/macOs use `ln -s ../../tool_box/source/tool_box tool_box`  to create symlink.
4. Continue to platform specific builds.

### Windows
1. From `asteroids/build` directory run `make_project.bat` to create a Visual Studio 2022 project inside `build/windows`
2. Open Visual Studio 2022 and build the project.

### macOS
Note: Unlike Windows, building the engine dependencies is a manual step until I solve the Premake dependencies issue with XCode in a similar fashion to Visual Studio solutions having multiple projects and setting build order based on `depends { }`. This means any time the engine is updated/modified you'll need to build each Turtle Brains, Internal Combusion Engine and Track Builder (in that order) before building Asteroids. Also not sure yet if I could create XCode builds from script to just have `make_turtle_brains.sh` work like it does on Linux.
1. for each (first Turtle Brains, then Internal Combusion Engine, then Track Builder)
  1. Run the `/build/make_project.sh` to create the XCode project inside `build/macos`
  2. Then Open the XCode project and build it for debug, release and public, 🤞 everything goes to plan.
2. From `asteroids/build` directory run `make_project.sh` to create an XCode project inside `build/macos`
3. Open the XCode project and build the project.

### Linux
Note: Unlike Windows, building the engine dependencies is a manual step until I solve the Premake dependencies issue with GCC in a similar fashion to Visual Studio solutions having multiple projects and setting build order based on `depends { }`.  This means any time the engine is updated/modified you'll need to build each Turtle Brains, Internal Combusion Engine and Track Builder (in that order) before building Asteroids.
1. for each (first Turtle Brains, then Internal Combusion Engine, then Track Builder)
2. Run `turtle_brains/build/make_turtle_brains.sh` _(will build debug, release and public)_
3. Run `ice/build/make_ice.sh` _(will build debug, release and public)_
4. Run `track_builder/build/make_project.sh --linux --debug`
5. Run `track_builder/build/make_project.sh --linux --release`
6. Run `track_builder/build/make_project.sh --linux --public`

1. From the `asteroids/build` directory run `make_project.sh --linux`

## Credits

