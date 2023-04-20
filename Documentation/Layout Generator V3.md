# This doc is work in progress! #

# Layout (Generator) V3 Documentation #
Note: This is a developer documentation. A basic knowledge of Unreal Engine is required.

## Table of Content: ##
- Overview
- DataTable Setup
- [FLayoutCellGenerationSettings](https://github.com/RadioArtz/scp-project-unreal/blob/main/Source/scppu/Public/Layout/LayoutStructs.h#L115) struct and its properties
- [ALayout](https://github.com/RadioArtz/scp-project-unreal/blob/main/Source/scppu/Public/Layout/Layout.h) nodes
- [ULayoutCell](https://github.com/RadioArtz/scp-project-unreal/blob/main/Source/scppu/Public/Layout/LayoutCell.h) nodes
- [ULayoutSublevelInterface](https://github.com/RadioArtz/scp-project-unreal/blob/main/Source/scppu/Public/Layout/LayoutSublevelInterface.h) usecase
- [ULayoutGeneratorWFC](https://github.com/RadioArtz/scp-project-unreal/blob/main/Source/scppu/Public/Layout/LayoutGeneratorWFC.h) nodes

## Overview ##
WIP

## DataTable Setup ##
The DataTable has to use [FLayoutCellGenerationSettings](https://github.com/RadioArtz/scp-project-unreal/blob/main/Source/scppu/Public/Layout/LayoutStructs.h#L115) as its row struct. It holds all possible rows/levels for generation on an [ALayout](https://github.com/RadioArtz/scp-project-unreal/blob/main/Source/scppu/Public/Layout/Layout.h). 

## Struct `FLayoutCellGenerationSettings` ##
This struct defines the generation settings for a layout cell, and is used as a row in a data table.

### Properties ###
| Property | Type | Description |
|----------|------|-------------|
| `Levels` | `TArray< TSoftObjectPtr<UWorld> >` | Different level variations for more variety (chooses one randomly). |
| `RequiredInstances` | `int32` | Minimum instances that will be generated. |
| `MaximumInstances` | `int32` | Maximum instances that will be generated. Must be >= 'Required Instances'. |
| `SpawnPoolEntries` | `int32` | How present this row is inside the spawn pool (higher value => higher chance to generate). |
| `HasConnections` | `FLayoutCellSides` | The sides where the cell can connect to other cells. |
| `DisableNeighbouringCells` | `FLayoutCellSides` | The sides where the neighboring cells are not allowed to generate. Must not conflict with 'Has Connections'. |
| `PreSpawnValidators` | `TArray< TSubclassOf<UBaseLayoutSpawnValidator> >` | Validates a spawn location before setting the cell to this row. Not all cells have been generated during this time. They are AND gated. Must not contain 'None'. |
| `PostSpawnValidators` | `TArray< TSubclassOf<UBaseLayoutSpawnValidator> >` | Validates a spawn location after all cells have been generated. They are AND gated. Must not contain 'None'. |
| `bLevelAlwaysVisible` | `bool` | Whether the level is always visible. |
| `bLevelAlwaysLoaded` | `bool` | Whether the level is always loaded. |

## Actor `ALayout` ##
The main class is [ALayout](https://github.com/RadioArtz/scp-project-unreal/blob/main/Source/scppu/Public/Layout/Layout.h). It is the only actor class, meaning its the only class that actually interacts with the world. It owns a map of [[FIntVector2](https://github.com/RadioArtz/scp-project-unreal/blob/main/Source/scppu/Public/Layout/LayoutStructs.h#L12) : [ULayoutCell](https://github.com/RadioArtz/scp-project-unreal/blob/main/Source/scppu/Public/Layout/LayoutCell.h)]. It is responsible for providing world context to every [ULayoutCell](https://github.com/RadioArtz/scp-project-unreal/blob/main/Source/scppu/Public/Layout/LayoutCell.h) as well as inter-cell communication. It also holds a reference to the datatable.

## Object `ULayoutCell` ##
The [ULayoutCell](https://github.com/RadioArtz/scp-project-unreal/blob/main/Source/scppu/Public/Layout/LayoutCell.h) is an object class. It holds a row from the datatable of its owner ([ALayout](https://github.com/RadioArtz/scp-project-unreal/blob/main/Source/scppu/Public/Layout/Layout.h)), can tell if a row is valid for this cell, and has ownership to the spawned level instance (called sublevel). When loading the sublevel, it calls a function from [ULayoutSublevelInterface](https://github.com/RadioArtz/scp-project-unreal/blob/main/Source/scppu/Public/Layout/LayoutSublevelInterface.h) onto every actor inside this sublevel. It also contains various helper-functions for better interaction with the sublevel.

## Interface `ULayoutSublevelInterface` ##
The [ULayoutSublevelInterface](https://github.com/RadioArtz/scp-project-unreal/blob/main/Source/scppu/Public/Layout/LayoutSublevelInterface.h) is an interface that allows actors of sublevels to communicate with the layout ([ALayout](https://github.com/RadioArtz/scp-project-unreal/blob/main/Source/scppu/Public/Layout/Layout.h)) and its owner ([ULayoutCell](https://github.com/RadioArtz/scp-project-unreal/blob/main/Source/scppu/Public/Layout/LayoutCell.h)). It also provides a unique seed for every actor.

## Object `ULayoutGeneratorWFC` ##
The [ULayoutGeneratorWFC](https://github.com/RadioArtz/scp-project-unreal/blob/main/Source/scppu/Public/Layout/LayoutGeneratorWFC.h) is an object class and is responsible for actually generating a proper layout onto [ALayout](https://github.com/RadioArtz/scp-project-unreal/blob/main/Source/scppu/Public/Layout/Layout.h) . It generates a layout via the Wavefunction Collapse Algorithm, and supports both sync and async mode. It must be created manually. No other layout class references this class and thus can be easily swaped out with a diffrent generator.
