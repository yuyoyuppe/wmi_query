# Goal

The main goal is to write this:
```cpp 
void good_code()
```
Instead of that:
```cpp
void bad_code()
```
# Description

- Modern C++ library for a subset of WMI API
- Strong typing: WMI classes are mapped to c++ classes through code generation
- Ability to expose UTF-8 strings only

# Usage
## Prerequisites
- VS2017+ compiler and `/std:c++17` switch
- [premake5](https://github.com/premake/premake-core/releases)
## Build & Run
```cmd
$ cd modern_wmi
$ premake5 vs2017
$ msbuild /p:Configuration=Release build\modern_wmi.sln
$ 
```

# Philosophy

This project's programming model draws inspiration from Haskell evaluation strategy, i.e. I don't implement anything unless I really need it.

