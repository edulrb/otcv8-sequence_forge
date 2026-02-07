# OTCv8-Dev + Sequence Forge (Sources)



This repository is a modified version of **OTClientv8** prepared to receive animations from the [Sequence Forge Tool](https://github.com/edulrb/otcv8-sequence_forge/releases/tag/tool). This engine modification allows for advanced sprite manipulation, including frame-by-frame offsets and custom animations defined via OTML.



## 🛠 Sequence Forge Integration



The system is triggered via the Lua function `playSequence(id)`. While this repository contains the client-side reception logic, the server-side implementation for **TFS 0.3.6** (used during development) will be released soon.



### Installation and Configuration



The necessary files are already available in this repository. To set up the animations:

1. **Animation Data**: Create and edit your sequences in `otml files/sequences.otml`.

2. Then place the sequences.otml file in the `client/data/things/{version}/` folder.

3. **Loading**: You must configure `modules/game_things/things.lua` to load the OTML file. You can find a pre-configured `things.lua` in this repository to use as a base or inspiration.



### ⚠️ Important: Rendering Configuration



To prevent sprites from being "cut off" or drawing under the ground during large offsets, you **must** enable the following feature in your client's `modules/gamelib/features.lua`:



```lua

g_game.enableFeature(GameMapDrawGroundFirst)

