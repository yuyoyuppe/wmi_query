workspace "modern_wmi"
configurations {"Debug", "Release"}
language "C++"
system "windows"
architecture "x86_64"
cppdialect "C++17"
startproject "tests"

paths = {
  core = "src/core/",
  api_generator = "src/api_generator/",
  generated_api = "src/generated_api/",
  tests = "src/tests/",
  build = "build/",
  deps = {
    pugixml = "deps/pugixml/src/"
  }
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

local function make_common_project_conf(src_path, use_pch)
  if use_pch then
    pchheader "pch.h"
    pchsource (src_path .. "pch.cpp")
  end
  flags {"FatalWarnings", "MultiProcessorCompile"}
  includedirs{src_path, paths.build}
  basedir (src_path)
  targetdir (paths.build .. "bin")
  objdir (paths.build .. "obj")
  files {src_path .. "**.cpp", src_path .. "**.h"}
  linkoptions { "-IGNORE:4221" }
  filter "configurations:Debug"
    symbols "On"
    runtime "Debug"
    defines { "DEBUG" }
    targetsuffix "_d"
  filter "configurations:Release"
    -- symbols "Off"
    symbols "FastLink"
    defines { "NDEBUG" }
    runtime "Release"
    optimize "On"
end

project "core"
  kind "StaticLib"
  generate_constants_header({ use_exceptions = false, use_utf8_strings = true},
                            paths.build .. "core_common_constants.h")
  generate_constants_header({ debug = { log_verbosity_level = "log_verbosity::verbose" }, release = { log_verbosity_level = "log_verbosity::info" }},
                            paths.build .. "core_log_constants.h")
  defines
  {
    "_SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING",
  }
  links {"wbemuuid.lib", "pugixml"}
  includedirs {paths.deps.pugixml}
  make_common_project_conf(paths.core, true)

project "api_generator"
  kind "ConsoleApp"
  generate_constants_header({ wmi_path = '"' .. path.getabsolute(paths.generated_api) .. '"' },
                            paths.build .. "generator_common_constants.h")
  links { "core", "pugixml" }
  includedirs {paths.core, paths.deps.pugixml}
  make_common_project_conf(paths.api_generator, true)

project "generated_api"
  kind "StaticLib"
  links { "core" }
  defines {"_CRT_SECURE_NO_WARNINGS"}
  buildoptions {"/bigobj"}
  includedirs {paths.core, paths.deps.pugixml}
  make_common_project_conf(paths.generated_api, true)
  optimize "Size"

project "tests"
  kind "ConsoleApp"
  links { "generated_api" }
  includedirs {paths.core, paths.generated_api, paths.deps.pugixml}
  make_common_project_conf(paths.tests, true)

project "pugixml"
  kind "StaticLib"
  make_common_project_conf(paths.deps.pugixml)