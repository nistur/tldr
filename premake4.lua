--[[
-- TLDR
-- TINY LITTLE DOM (READER) LIBRARY
--]]

--[[
-- insert zlib license here
--]]

solution "tldr"
configurations { "Debug", "Release" }
language "C++"
includedirs { "include" }
files "include/**h"

configuration "Debug"
defines { "DEBUG" }
flags { "Symbols" }
targetdir "build/debug"

configuration "Release"
defines { "NDEBUG" }
flags { "OptimizeSpeed",
	"ExtraWarnings",
	"FatalWarnings",
	"NoFramePointer"}
targetdir "build/release"

project "tldr"
kind "StaticLib"
files { "src/tldr.cpp" }

project "tests"
kind "ConsoleApp"
files "tests/**.cpp"
links { "tldr" }
configuration "Debug"
postbuildcommands("build/debug/tests")
configuration "Release"
postbuildcommands("build/release/tests")
