workspace "modern_wmi"
configurations {"Debug", "Release"}
language "C++"
system "windows"
architecture "x86_64"
cppdialect "C++17"

paths = {
  core = "src/core/",
  api_generator = "src/api_generator/",
  generated_api = "src/generated_api/",
  tests = "src/tests/",
  build = "build/",
}
location(paths.build)

newaction {
	trigger = "clean",
	description = "Clean all generated build artifacts",
	onStart = function()
    os.rmdir(paths.build)
  end
}

local function generate_constants_header(constants, file_path)
  local content = ""
  local function add(line) content = content .. line .. "\n" end
  add("#pragma once")
  add("namespace konst {")

  local function add_table(tbl)
    for name, v in pairs(tbl) do
      local v_type = type(v)
      if v_type ~= "table" and v_type ~= "function" and v_type ~= "nil" then
        add("constexpr auto " .. name .. " = " .. tostring(v)  .. ";")
      end
    end
  end
  add_table(constants)
  if type(constants.debug) == "table" then
    add("#ifdef _DEBUG")
    add_table(constants.debug)
    add("#endif")
  end

 if type(constants.release) == "table" then
    add("#ifndef _DEBUG")
    add_table(constants.release)
    add("#endif")
 end
 add("} // namespace konst")
  local tmp_filename = os.tmpname()
  local tmp_file = io.open(tmp_filename, "w")
  io.writefile(tmp_filename, content)
  if not os.isfile(file_path) or not os.comparefiles(tmp_filename, file_path) then
    os.mkdir(path.getdirectory(file_path))
    os.copyfile(tmp_filename, file_path)
    tmp_file:close()
  end
  os.remove(tmp_filename)
end

project "core"
  generate_constants_header(
    { use_exceptions = false, use_utf8_strings = true},
    paths.build .. "core_common_constants.h")
  generate_constants_header(
    { debug = { log_verbosity_level = "log_verbosity::verbose" }, release = { log_verbosity_level = "log_verbosity::info" }},
    paths.build .. "core_log_constants.h"
  )
  defines
  {
    "_SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING",
  }

  links {"wbemuuid.lib"}
  targetdir (paths.build .. "bin")
  objdir (paths.build .. "obj")
  basedir (paths.core)
  files {paths.core .. "**.cpp", paths.core .. "**.h"}
  includedirs {paths.core, paths.build}
  pchheader "pch.h"
  pchsource (paths.core .. "pch.cpp")
  kind "StaticLib"
  filter "configurations:Debug"
    symbols "On"
    runtime "Debug"
    defines { "DEBUG" }
    targetsuffix "d"
  filter "configurations:Release"
    symbols "Off"
    defines { "NDEBUG" }
    runtime "Release"
    optimize "On"

project "api_generator"
  generate_constants_header(
    { wmi_path = '"' .. path.getabsolute(paths.generated_api) .. '"' },
      paths.build .. "generator_common_constants.h")
  targetdir (paths.build .. "bin")
  objdir (paths.build .. "obj")
  links { "core" }
  basedir (paths.api_generator)
  files {paths.api_generator .. "**.cpp", paths.api_generator .. "**.h"}
  includedirs {paths.api_generator, paths.core, paths.build}
  pchheader "pch.h"
  pchsource (paths.api_generator .. "pch.cpp")
  kind "ConsoleApp"
  filter "configurations:Debug"
    symbols "On"
    runtime "Debug"
    defines { "DEBUG" }
    targetsuffix "d"
  filter "configurations:Release"
    symbols "Off"
    defines { "NDEBUG" }
    runtime "Release"
    optimize "On"

project "generated_api"
  targetdir (paths.build .. "bin")
  objdir (paths.build .. "obj")
  links { "core" }
  basedir (paths.generated_api)
  files {paths.generated_api .. "**.cpp", paths.generated_api .. "**.h"}
  includedirs {paths.generated_api, paths.core, paths.build}
  pchheader "pch.h"
  pchsource (paths.generated_api .. "pch.cpp")
  kind "StaticLib"
  filter "configurations:Debug"
    symbols "On"
    runtime "Debug"
    defines { "DEBUG" }
    targetsuffix "d"
  filter "configurations:Release"
    symbols "Off"
    defines { "NDEBUG" }
    runtime "Release"
    optimize "On"

project "tests"
  targetdir (paths.build .. "bin")
  objdir (paths.build .. "obj")
  links { "generated_api" }
  basedir (paths.tests)
  files {paths.tests .. "**.cpp", paths.tests .. "**.h"}
  includedirs {paths.tests, paths.core, paths.generated_api}
  pchheader "pch.h"
  pchsource (paths.tests .. "pch.cpp")
  kind "ConsoleApp"
  filter "configurations:Debug"
    symbols "On"
    runtime "Debug"
    defines { "DEBUG" }
    targetsuffix "d"
  filter "configurations:Release"
    symbols "Off"
    defines { "NDEBUG" }
    runtime "Release"
    optimize "On"