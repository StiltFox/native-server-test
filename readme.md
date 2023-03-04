# Stilt Fox™ C++ Rest Api

This is a small project made to demonstrate how rest APIs work at a low level.
This code is provided without warranty or any support.

Stilt Fox™ is not liable for any damages done to your hardware. For more information see LICENSE file.

Stilt Fox™ is trademarked. You may not use the Stilt Fox name, however this code is free to reference and use.

# Compiling the API

Supported Systems:
- Linux (tested on Pop_Os and Ubuntu)
- MacOs

## Pre Requisites
- Cmake must be installed on the system
    - you can check if it's installed by using
    >cmake --version
    - must be version 3.0.0 or higher
- A C++ compiler should be installed
    - Clang and G++ are good examples

## Not Required but Recommended
- Visual Studio Code
    - Install CMake and C++ Plugins

## Compiling with Visual Studio
At the bottom of the application, there should be a play button. When you click on it it will ask you to select a tool chain if you have not already set one up. Be sure to pick your latest c++ compiler.

## Compiling with Command Line
- Linux
> chmod +x build.sh
>
> ./build.sh
- Mac
> chmod +x build_mac.sh
>
>./build_mac.sh

## A Note on Windows
While Windows is currently not supported natively (Maybe in the future), this program should be able to run under WSL. This has not been tested however. If using WSL follow instructions for Linux.