require 'modules/glew'
require 'modules/glfw'
require 'modules/glm'
require 'modules/soil'
require 'modules/json'

solution("CPURenderer")
	language "C++"
	debugdir "$(OutDir)"
	objdir "build/"
	targetdir "$(SolutionDir)../game/"
	targetname "$(ProjectName)_$(Configuration)_$(Platform)"
	
	configurations{
		"Debug",
		"Release",
	}
	
	platforms{
		"Win32",
		"x64",
	}
	
	configuration "Debug"
		symbols "On"
		defines{
			"DEBUG"
		}
		
	configuration "Release"
		optimize "On"
		defines{
			"RELEASE",
			"NDEBUG"
		}
		
	configuration{}
	
project("CPURenderer")
	kind "ConsoleApp"

	includedirs{
		"code/",
	}
	files{
		"code/**.h",
		"code/**.cpp",
	}
	
	enable_glew()
	enable_glfw()
	enable_glm()
	enable_soil()
	enable_json()