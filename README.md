# QWC-Console
Developer Console restoration project for Harry Potter: Quidditch World Cup

## Building

First generate the Visual Studio 2019 project/solution files:

```cmd
mkdir build
cd build
cmake -G "Visual Studio 15" ..
```

Compile the solution from the Developer Command Prompt:

```cmd
devenv QWC_Console.sln /Build Release 
```

## Usage

- Drop d3d8.h file in the same directory as QWC.exe
- Toggle console in game with tilda key (ie. Source Engine games)
- TAB key can be used for autocompletion, command arguments will be displayed for complete commands
- Hint: Set game resolution to 1024x768 in order to avoid clipping issues

## Sources
IMGUI Dx8 back-end from THRotator Project: https://github.com/massanoori/THRotator/

d3d8.dll wrapper: https://github.com/ThirteenAG/d3d8-wrapper/
