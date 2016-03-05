
# clockUtils #

Welcome to **clockUtils**!

clockUtils is a collection of lightweight libraries to solve common tasks. We designed it to be used when you don't want to write the code by yourself and don't need some odd features.

## Features ##

**General**

 * Easy to use and lightweight libraries for often used problems. Optimized for simple usage and performance.

**Argument Parser**

 * Define variables and parse the parameters in a commandline or string buffer like most unix-like programs.

**Compression**

 * minimize strings with different compression algorithms

**Ini File Parser**

 * read and write ini files
 * custom types are possible due to C++ templates

**Sockets**

 * simple and fast C++ wrapper for C sockets
 * a small test with our own software was more than twice as fast with our TcpSocket instead of boost.asio's tcp socket


## Platforms ##

clockUtils has been tested on:

  * Linux
  * Windows
  * Android

## Requirements ##

clockUtils has nearly no requirements. We use CMake (2.8+) for the build system and a C++11 capable compiler such as g++ 4.7 or Visual Studio 13 (or higher).

## Documentation ##

The doxygen documentation can be found on our homepage: http://clockwork-origins.de/en/clockUtils/docu

## Building from source ##

To build clockUtils from source you need cmake and a C++11 compiler. For a complete library setup just use

$ cmake .
$ make

You can enable/disable all libraries using -DWITH_LIBRARY_<LIBRARYNAME>=ON/OFF. Tests can be enabled using -DWITH_TESTING=ON. This requires gtest on your system (or you build it with the appropriate dependency build script in the dependencies directory).

## Contributing Code ##

We welcome patches. We will set up a Contributor Agreement soon to take care of the legal aspect of contributions.
