# Contributing to SCP: Project Unreal

## Welcome!
First of all, everyone is allowed to contribute and we are thankful for every commit. However, to still be able to steer this project and to avoid duplicated work, we highly encourage you to discuss changes and additions with us on our [Discord Server](https://discord.gg/Zs2UMr8) first!

Please note we have a code of conduct and we plead you to follow it.

## Prerequisites
- Unreal Engine 4.27 (prebuild binaries by epic games are sufficient)
- MSVC, G++, or any other equivalent C++ compiler with support for C++ 17

## Code of Conduct:
To make the life of all of us more comfortable, we want to ensure that our project is neat and tidy (for the most part at least). This is why a code of conduct is needed.

- Follow the [Recommended Asset Naming Conventions](https://docs.unrealengine.com/4.27/en-US/ProductionPipelines/AssetNaming/) by Epic Games. The only current exception is that instead of `WBP_` (Widget Blueprint) we use `W_` (Widget).
- Avoid modifying unnecessary files. If you need to make changes to a file, don't feel afraid to do so. However if a change to a file is miniscule or unnecessary, we plea you revert and not commit it. This is to reduce the chance of merge conflicts, which are a rather common occurence due to unreal engine's usage of binary files for assets.
- Split your pull requests based on the features they implement. We rather would have multiple smaller pull requests with a single feature than one big pull request with all features.
- Stay conform to our [License](https://github.com/RadioArtz/scp-project-unreal/blob/main/License.txt), this includes all assets you may import.
- Do not use marketplace assets.

### When working with blueprints:
- Avoid unnecessary cast to other blueprint classes. This only increases the potential memory footprint and may end in a disaster some day.
- Do not make use of collapsed graphs.
- Name your variables, functions, etc. appropriately.
- Try to make your code easy on the eyes (aka. no spaghetti).

### When working with C++:
- Follow the [Coding Standard](https://docs.unrealengine.com/4.27/en-US/ProductionPipelines/DevelopmentSetup/CodingStandard/) by Epic Games. The only current exceptions are: const correctness is not really a thing here, so is encapsulation for cpp only classes. This may change in the future, but we won't enforce it as of now.
- Use tabs as indenting method.
- Try to avoid unnecessary includes.
- `#pragma once`


While we're not super strict about these rules, we may still refrain from merging your pull request if you fail to follow them. We are always open for communication and exceptions, likewise adaptations to these rules are not few and far between. 
