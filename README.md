## BozSceneTreeEditor

[![Version: 1.0.2](https://img.shields.io/badge/version-1.0.2-lightgrey.svg)](https://github.com/djbozkosz/BozSceneTreeEditor)
[![Game: Mafia](https://img.shields.io/badge/game-mafia-red.svg)](https://mafiagame.com)
[![Qt: 5](https://img.shields.io/badge/qt-5-green.svg)](https://www.qt.io)
[![License: GPL v3](https://img.shields.io/badge/license-GPL%20v3-blue.svg)](https://github.com/djbozkosz/BozSceneTreeEditor/blob/master/LICENSE)

### Introduction:
Tool for full nodes control in Mafia scene files. Common operations are defined like add, edit, remove, move nodes and their values.
Whole data definition is stored in `definitions.txt` file.

<img src="https://i.postimg.cc/g03LX2jf/editor.png" alt="Editor">

### Usage:
Open or create new file from `File` menu. If you will create the new one then you can import new scene structure from `dnc` directory.
While editing you can drag and drop nodes to proper place. You can also copy nodes between opened tabs.

You can create empty nodes from `Edit` menu (or use right click) or just import predefined nodes from `dnc` directory.

Workflow note: don't forget to rename node after duplication.

### Build
Download and install <a href="https://www.qt.io" target="_blank">Qt library</a>. Open project `pro` file and
in build setup change the working directory to project directory. Then build the project.

### Disclaimer
Make sure you will backup your file before saving!

This is not the most beautiful code I ever wrote but I did it fast. Enjoy :)
