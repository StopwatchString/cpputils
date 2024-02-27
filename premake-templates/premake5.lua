--Bootstrap C++ console app premake. 
--Put this in /${ProjectName}/build and run your premake command.

--Customizable Entries--
local solutionName = "NewSolution"
local projectName = "NewProject"
------------------------

local solutionLocation = ".."
local srcDir = solutionLocation .. "/src"
local resDir = solutionLocation .. "/res"
local libDir = solutionLocation .. "/lib"
local libDebugDir = solutionLocation .. "/lib/Debug" 
local libReleaseDir = solutionLocation .. "/lib/Release"
local includeDir = solutionLocation .. "/include"
local targetDir = solutionLocation .. "/bin"
local objDir = "/obj"

--Useful directories.
os.mkdir(srcDir)
os.mkdir(resDir)
os.mkdir(libDebugDir)
os.mkdir(libReleaseDir)
os.mkdir(includeDir)

workspace(solutionName)
	location(solutionLocation)
	configurations({ "Debug", "Release" })
	system "Windows"
	architecture "x64"

	project(projectName)
		location(solutionLocation)
		kind("ConsoleApp")
		language("C++")
		cppdialect("C++20")

		links({})
		files({
			srcDir .. "/*.c",
			srcDir .. "/*.cpp",
			includeDir .. "/**.h",
			includeDir .. "/**.hpp"
		})
		includedirs({includeDir})

		filter("configurations:Debug")
			defines({ "DEBUG" })
			libdirs({libDebugDir})
			symbols("ON")

			targetdir(targetDir .. "/Debug")
			objdir(targetDir .. "/Debug" .. objDir)

		filter("configurations:Release")
			defines({ "NDEBUG" })
			libdirs({libReleaseDir})
			symbols("OFF")
			optimize("ON")

			targetdir(targetDir .. "/Release")
			objdir(targetDir .. "/Release" .. objDir)
