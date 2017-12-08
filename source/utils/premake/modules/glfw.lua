GLFW_DIR = "$(SolutionDir)utils/deps/"

function enable_glfw()
	filter {"platforms:x64"}
		includedirs { GLFW_DIR .. "glfw64/include/" }
		libdirs { GLFW_DIR .. "glfw64/lib-vc2015/" }
		
	filter {"platforms:Win32"}
		includedirs { GLFW_DIR .. "glfw32/include/" }
		libdirs { GLFW_DIR .. "glfw32/lib-vc2015/" }
	
	filter {}

	links { "opengl32.lib", "kernel32.lib", "user32.lib", "glfw3.lib" }
end