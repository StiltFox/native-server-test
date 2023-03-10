# Stilt Fox™ C++ Rest Api

This is a small project made to demonstrate how rest APIs work at a low level.
This code is provided without warranty or any support.

Stilt Fox™ is not liable for any damages done to your hardware. For more information see LICENSE file.

Stilt Fox™ is trademarked. You may not use the Stilt Fox™ name, however this code is free to reference and use.

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

# Project Structure
## Project Root
### readme.md
This is the file that you are currently reading. It's intended to help people looking at your project get information about how to build or execute your project. All sorts of things can be included here from help documentation to legal information, and even this tutorial. A readme.md is essential for communicating to people looking at your project.

### LICENSE
This file contains legal information about copyright and permissions granted to the reader. Are you allowed to use the code for commercial purposes? What rights do you have as a consumer of this code base? ect... It's boring but necessary.

### build.sh
This is a helper script for Linux systems. This will create a build folder and run CMake to compile the program for you. No command line experience required. On some GUI Linux systems you can even double click the script to execute it, given you granted it the proper permissions.

### build_mac.sh
This is the same thing as build.sh, but for Mac computers. This is important as the build environments are not the same. Be sure to run the proper helper file if you wish to compile this way.

### .gitignore
This file prevents git from uploading things like IDE preferences or large binary files. A healthy gitignore will get your repository to basically only store the code, and files relevant to running your repository.

### CMakeLists.txt
This file is like the gradle file, but for C++. It outlines dependencies both external and internal and tells the compiler how to construct the project. This is often easier than writing the commands for g++ manually.

### Mac.cmake
Mac specific cmake configurations are included here. It's not fully developed for cross compiling yet, but we're getting there.

### main.cpp
This is the main code file that makes up our executable. The main function is found here and this code file is also full of comments on how the api works. These comments assume that you don't know a lot about C++. I wanted to make something beginner friendly as a lot of developers go to boot camps now a days. We hope to spread the love of computer science with this little project.

### modules
This folder contains the other libraries we wrote to make this main program function. If there's an object or function not defined in main, it's either here or in a system file somewhere.

## Modules
Each module is constructed of 4 different files:
- A CMakeLists.txt
- a header file
- a source file
- a test source file

The naming convention used has the module folders be lower case, but the inner files be camel case like object definitions. Other than the casing, both the .cpp and .hpp files will match the name of the folder with the test file having Test appended to the end of the name.

### CMakeLists.txt
This CMake file does not do much and acts more as a passthrough. As each subdirectory must have it's own CMakeLists.txt, this one simply adds all the other modules to the subdirectories searched by CMake.

### httpmessage
This module contains the code for parsing and constructing Http request and responses.

### socket
This module contains the code for opening, closing, reading and sending to sockets.

### stringmanip
This module contains some helper functions used in string parsing.