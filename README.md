# Silicate + Scripting

## Is this safe?
This fork is fully open source. You can compare every change against upstream Silicate yourself using Git.
For maximum trust, review the diff and build from source locally instead of using prebuilt binaries.

### See what's different between original silicate and this fork
Install git bash: https://git-scm.com/
1. Create a folder, open it and open **Git Bash** inside it (right-click in explorer)
2. Clone this repository (this will copy the code), using the command `git clone https://github.com/luaurocks/silicate-scripting.git` (you can get the link in this repository's page clicking Code and copying git link)
3. Type `cd silicate-scripting` to navigate to the cloned repository
4. Add original silicate source code by `git remote add silicate https://git.silicate.dev/silicate/silicate.git`. Then, add the files from it with `git fetch silicate`
5. Now, you can see the difference between these repositories with `git diff main silicate/main`

## Building it yourself
This repository has **Releases** section. You can download it from there, but be aware that the files are compiled by the owner of this fork. **Always check the commit history to make sure you're not compiling viruses.**

### Preparing to compile
1. If you haven't cloned this repository, do it (explained above in first 3 steps)
2. Navigate to lib folder (`cd lib`)
> [!IMPORTANT]
> Before cloning libraries, make sure they go into lib folder. Otherwise, the compiler wouldn't be able to find them and fail.
> If you did clone them into the root folder, you can move them to lib (drag or whatever)
3. Now you need to clone tabby, this is required by silicate to render UI: `git clone -b legacy-v1 https://github.com/silicate-bot/tabby.git`
4. Then, you need to clone luau, this is required by the scripting feature of this fork: `git clone https://github.com/luau-lang/luau.git`

### Getting a compiler
Install these first:
- CMake: https://cmake.org/download/
- Ninja: https://ninja-build.org/
- Visual Studio Build Tools 2022: https://visualstudio.microsoft.com/visual-cpp-build-tools/
> [!IMPORTANT]
> When installing Visual Studio Build Tools, select **Desktop development with C++**, **MSVC compiler**, **Windows SDK**.
> After you installed it, make sure to open `x64 Native Tools Command Prompt for VS 2022` so the compiler is properly configured. You can find it in start menu.

### Installing Geode SDK:
This project uses v5.0.0. Refer to https://docs.geode-sdk.org/
Instead of `geode sdk install`, use `geode sdk install 5.0.0` (or replace install with update if it conflicts). Every time you update the SDK, you should update its prebuilt binaries too `geode sdk install-binaries`.
Afterwards you can do the usual, what the documentation of Geode tells you.

### Compiling
This repository contains `build.bat` and `build-rel.bat` for convenience. Check what they do by right-clicking then pressing Edit, before running.

But if you'd like to do it manually:
1. Open `x64 Native Tools Command Prompt for VS 2022`, then type `cd <full path to silicate-scripting folder>`
2. To configure the project, do `cmake -S . -B build -G "Ninja" -DCMAKE_C_COMPILER=clang-cl -DCMAKE_CXX_COMPILER=clang-cl -DCMAKE_BUILD_TYPE=Release` (or RelWithDebInfo)
3. Then, to compile do `cmake --build build --config Release` (or RelWithDebInfo)
4. Geode's CMake macro will automatically add the mod to GD, if you have set up a profile in Geode SDK CLI.

---

# Scripting API

## game
| Field                  | Description                  |
|------------------------|------------------------------|
| `game.tick: number`    | Current tick value.          |
| `game.tps: number`     | Current TPS value.           |
| `game.getInputs(): { {holding: boolean, isPlayer2: boolean} }` | Returns a table of queued inputs. Do not modify this table! To add inputs, use corresponding player methods. |
| `game.onUpdate(callback: ()->()): void` | Binds a function to the game loop. Runs every physics tick before processing queued inputs. |

Example:
```luau
game.onUpdate(function()
  local inputs = game.getInputs()
  if #inputs ~= 0 then
    log("Player has started holding or released during this frame")
  end
end)
```

## player
Index player with a number to get the player object. e.g. `player[1].x`
| Field                        | Description                  |
|------------------------------|------------------------------|
| `player[1].x: number`        | X position of player         |
| `player[1].y: number`        | Y position of player         |
| `player[1].yVel: number    ` | Y velocity of player         |
| `player[1].hold(): void`     | Adds hold input of that player |
| `player[1].release(): void`  | Adds release input of that player |

Example:
```luau
-- Auto swift
game.onUpdate(function()
  local inputs = game.getInputs()
  local lastInput = inputs[#inputs]
  if lastInput.holding and not lastInput.isPlayer2 then
    player[1].release()
  end
end)
```

## ui
These elements must be created at the top of the script. If you create them in an update loop, they will keep being created every frame.
| Field                        | Description                  |
|------------------------------|------------------------------|
| `ui.button(label: string, callback: ()->()): void` | Creates a button. When clicked `callback` is called |
| `ui.drag(label: string, value: number, min: number, max: number, speed: number, callback: (val: number)->()): ()` | Creates number input field. Currently the displayed accuracy is 2 digits after decimal point. Callback is called when the value is changed. |
| `ui.checkbox(label: string, initialValue: boolean, callback: (val: boolean)->()): ()` | Creates a checkbox. When clicked `callback` is called |

Example:
```luau
-- Autoclicker
local hold = 1
ui.drag("Hold frames", hold, 0, math.huge, 1, function(val)
	hold = math.floor(val)
end)
local release = 1
ui.drag("Release frames", release, 0, math.huge, 1, function(val)
	release = math.floor(val)
end)

local waiting = 0
local releasing = false
game.onUpdate(function()
  waiting -= 1
  if waiting > 0 then return end
  if releasing then
    releasing = false
    waiting = hold
    player[1].release()
  else
    releasing = true
    waiting = release
    player[1].hold()
  end
end)
```
