## BozSceneTreeEditor

[![Version: 1.1.0](https://img.shields.io/badge/version-1.1.0-lightgrey.svg)](https://github.com/djbozkosz/BozSceneTreeEditor)
[![Game: Mafia](https://img.shields.io/badge/game-mafia-red.svg)](https://mafiagame.com)
[![Qt: 5](https://img.shields.io/badge/qt-5-green.svg)](https://www.qt.io)
[![License: GPL v3](https://img.shields.io/badge/license-GPL%20v3-blue.svg)](https://github.com/djbozkosz/BozSceneTreeEditor/blob/master/LICENSE)

### Introduction:
Boz Scene Tree Editor is a modding tool for the first Mafia game. It brings mostly full control over the various tree and node based game files - `scene2.bin`, `cache.bin`, `effects.bin` and additionally `dnc` files.

- **Scene2.bin** - Main level file, describing player, objects placement, lights, sounds, scripts and more.
- **Cache.bin** - City file containing simple objects, which are managed in groups (city blocks). These objects are static and cannot be accessed from the main scene file or via scripts.
- **Effects.bin** - Scene file with the placement of the all static particle effects.
- **\*.dnc** - Snipped of the files above. Originally defined by the DCED scene editor as the format for storing exported/imported objects.

<!-- <img src="https://i.postimg.cc/g03LX2jf/editor.png" alt="Editor"> -->
<img src="https://i.postimg.cc/wjdJQxXx/image00-full.png" alt="Editor">

### Usage:
Open or create a new file from the `File` menu. When the new file is created, it is possible to import the new scene structure from the dnc directory.

While editing, drag and drop the nodes or copy paste them to the proper place. Even between the opened tabs. Empty nodes may be created from the Edit menu, via right click context menu or import predefined and custom dnc nodes.

> [!WARNING]
> Valid tree structure must be maintained, to keep compatibility with the game. For example: don't forget to rename node after duplication.
> Backup your scene files before saving!

### Build
Download and install <a href="https://www.qt.io" target="_blank">Qt library</a>. Open project `pro` file and change the working directory to project directory in the build setup. Then, build the project.

Enjoy. :)

<img src="https://i.postimg.cc/6pLZjsc5/image01-full.png" alt="Editor">
