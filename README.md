# POGLIB

A C++ library to load the contents of POG file into memory.
Types, expressions and predicates are represented using the
BAST library (available separately).

Dependencies:

* [BAST](https://github.com/CLEARSY/BAST): a C++ library to represent B Abstract Syntax Trees. Depends on Qt5Core, Qt5Xml.
  BAST is a git submodule.

* [TinyXML-2](https://github.com/leethomason/tinyxml2): "a simple, small, efficient, C++ XML parser that can be easily integrated into other programs".
  TinyXML-2 is also a submodule.

## Compiling

The build process is based on `cmake`, which produces suitable Makefiles from the `CMakeLists.txt` provided here.
To build the code, run the following commands

```sh
cmake -B build
cmake --build build
```

To update the repository and its submodules, use the following command:

```sh
git submodule update --init --recursive
```

## Contributing

We welcome external contributors to POGLIB!

Please carefully read the CONTRIBUTING.md file in this repository in case you consider contributing.

## Licensing

This software is copyright (C) CLEARSY 2023-2025. All rights reserved.

The source code is distributed under the terms of the GNU General Public Licence (GNU GPL) Version 3.
