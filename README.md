# Silicate + Scripting
This fork only adds the scripting tab. Scripts are usually stored in `%localappdata%/GeometryDash/geode-persistent/peony.silicate/scripts`.

Check out the [Scripting API Documentation](https://github.com/luaurocks/silicate-scripting/wiki/Scripting-API)!

## Is this safe?
This fork is fully open source. You can compare every change against upstream Silicate yourself using Git.
For maximum trust, review the diff and build from source locally instead of using prebuilt binaries.

### See what's different between original silicate and this fork
This tutorial will give you all lines changed compared to original Silicate. If you don't know C++, ask whoever or ChatGPT.
1. Install git bash: https://git-scm.com/
2. Create a folder, open it and open **Git Bash** inside it (right-click in explorer)
3. Clone this repository (this will copy the code), using the command `git clone https://github.com/luaurocks/silicate-scripting.git` (you can get the link in this repository's page clicking Code and copying git link)
4. Type `cd silicate-scripting` to navigate to the cloned repository
5. Add original silicate source code by `git remote add silicate https://git.silicate.dev/silicate/silicate.git`. Then, add the files from it with `git fetch silicate`
6. Now, you can see the difference between these repositories with `git diff main silicate/main`

## Building it yourself
This repository has **Releases** section. You can download it from there, but be aware that the files are compiled by the owner of this fork. **Commit history is fully public and every change is logged and reviewable.**

### Preparing to compile
1. If you haven't cloned this repository, do it (explained above in first 4 steps)
2. Navigate to lib folder: `cd lib`
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
