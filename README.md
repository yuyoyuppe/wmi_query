# Description

This library provides modern C++ API for WMI interface. The main goal is to write this:

```cpp
for(const auto& o : wmi::WmiMonitorID::get_all_objects())
  log(info, "got: %s", o.to_string().c_str());
```

instead of [that](https://docs.microsoft.com/en-us/windows/win32/wmisdk/example--getting-wmi-data-from-the-local-computer-asynchronously).

# Usage

## Prerequisites

- VS2022 compiler
- [premake5](https://github.com/premake/premake-core/releases)

## Build & Run

```cmd
$ cd wmi_query
$ premake5 vs2022 --classes=WmiMonitorID
$ msbuild /p:Configuration=Release build\modern_wmi.sln
```

Your newly generated API is ready: just use `wmi_classes.h` and `generated_api.lib` which don't require any additional dependencies!

#### Options

- `--classes` delimiter is a comma (`,` or ` `)
  - If not defined, will compile all available WMI classes