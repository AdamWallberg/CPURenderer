SOIL_DIR = "$(SolutionDir)utils/deps/soil/"

function enable_soil()
	includedirs { SOIL_DIR .. "src/" }

	filter {"platforms:Win32"}
		libdirs     { SOIL_DIR .. "lib/" }
	
	filter {"platforms:x64"}
		libdirs     { SOIL_DIR .. "lib64/" }
		
	configuration "Debug"
		links { "soild.lib" }
	configuration "Release"
		links { "soil.lib" }
	filter {}
end
