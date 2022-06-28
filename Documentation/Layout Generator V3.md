# This doc is work in progress! #

# Layout (Generator) V3 Documentation #
Note: This is a developer documentation. A basic knowledge of Unreal Engine is required.

## Table of Content: ##
- Overview
- DataTable Setup
- `FLayoutCellGenerationSettings` struct and its properties
- `ALayout` nodes
- `ULayoutCell` nodes
- `ULayoutSublevelInterface` usecase
- `ULayoutGeneratorWFC` nodes

## Overview ##
(datatable stuff here)

The layout itself is grid-based and split up in 2 diffrent classes (+ 1 interface). The generator itself is seperate.

The main class is `ALayout`. It is the only actor class, meaning its the only class that actually interacts with the world. It owns a map of `FIntVector2 : ULayoutCell`. It is responsible for providing world context to every `ULayoutCell` as well as inter-cell communication. It also holds a reference to the datatable.

The `ULayoutCell` is an object class. It holds a row from the datatable of its owner (`ALayout`), can tell if a row is valid for this cell, and has ownership to the spawned level instance (called sublevel). When loading the sublevel, it calls a function from `ULayoutSublevelInterface` onto every actor inside this sublevel. It also contains various helper-functions for better interaction with the sublevel.

The `ULayoutSublevelInterface` is an interface that allows actors of sublevels to communicate with the layout (`ALayout`) and its owner (`ULayoutCell`). It also provides a unique seed for every actor.

The `ULayoutGeneratorWFC` is an object class and is responsible for actually generating a proper layout onto `ALayout`. It generates a layout via the Wavefunction Collapse Algorithm, and supports both sync and async mode. It must be created manually. No other layout class references this class and thus can be easily swaped out with a diffrent generator.