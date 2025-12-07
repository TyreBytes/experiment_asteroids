--
-- Premake5 script for creating a Visual Studio, XCode or CodeLite workspace for the Asteroids project.
--   Requires Premake5 from: http://industriousone.com/
--
-- <!-- Copyright (c) 2024 Tyre Bytes LLC - All Rights Reserved -->
-----------------------------------------------------------------------------------------------------------------------

newoption {
	trigger = "web",
	description = "Chosen build system to override for web.",
	value = "",
}

newoption {
	trigger = "build-version",
	description = "Version being built, expecting major.minor.patch; --build-version=1.2.3",
	value = "0.0.0",
}

BUILD_VERSION = "0.0.0"

local file = io.open("version.txt", "r")
if (nil ~= file) then
	BUILD_VERSION = file:read("*line")
	file:close()
end

if _OPTIONS["build-version"] ~= nil and _OPTIONS["build-version"] ~= '' then
	BUILD_VERSION = _OPTIONS["build-version"]
end

BUILD_VERSION_MAJOR, BUILD_VERSION_MINOR, BUILD_VERSION_PATCH = string.match(BUILD_VERSION, "(%d+).(%d+).(%d+)")
print("Asteroids Version: " .. BUILD_VERSION_MAJOR .. "." .. BUILD_VERSION_MINOR .. "." .. BUILD_VERSION_PATCH)

local WINDOWS_SYSTEM_NAME = "windows"
local LINUX_SYSTEM_NAME = "linux"
local MACOS_SYSTEM_NAME = "macos"
local WEB_SYSTEM_NAME = "web"
--local MACIOS_SYSTEM_NAME = "macios"
--local ANDROID_SYSTEM_NAME = "android"

local PROJECT_NAME = "asteroids"
local SYSTEM_NAME = os.target():gsub("macosx", "macos")
if _OPTIONS["web"] then
	SYSTEM_NAME = WEB_SYSTEM_NAME
end

if _ACTION == "clean" then
	os.rmdir("../build/" .. WINDOWS_SYSTEM_NAME)
	os.rmdir("../build/" .. LINUX_SYSTEM_NAME)
	os.rmdir("../build/" .. MACOS_SYSTEM_NAME)
	os.rmdir("../build/" .. WEB_SYSTEM_NAME)
end

local TB_PLATFORM_DEFINE = "tb_" .. SYSTEM_NAME
local SCRIPT_EXTENSION = ".sh"

local TURTLE_BRAINS_DIRECTORY = "../../turtle_brains/"
if os.isdir("../externals/turtle_brains") then
	TURTLE_BRAINS_DIRECTORY = "../externals/turtle_brains/"
end

------------------------------------------------------------------------------------------------------------------------

workspace (PROJECT_NAME)
configurations { "debug", "development", "release", "public" }
startproject "asteroids"

if (WINDOWS_SYSTEM_NAME == SYSTEM_NAME) then
	SCRIPT_EXTENSION = ".bat"
	architecture "x86_64"
end

--------------------------------------------------------------------------------- Common Project Settings (ALL projects)
location ("../build/" .. SYSTEM_NAME)
language ("C++")
cppdialect "C++20"
runtime "Release" -- 2024-08-05: Always use the static release /MT C runtime libs
warnings ("Extra")
flags { "FatalCompileWarnings" }
debugdir "../run"

defines {
	TB_PLATFORM_DEFINE,
	"tb_%{cfg.buildcfg}_build",
	"build_major=" .. BUILD_VERSION_MAJOR,
	"build_minor=" .. BUILD_VERSION_MINOR,
	"build_patch=" .. BUILD_VERSION_PATCH
}

includedirs {
	"../source/",
	"../externals/includes/",
	TURTLE_BRAINS_DIRECTORY .. "source/",
	TURTLE_BRAINS_DIRECTORY .. "externals/includes/"
}

libdirs {
	"../externals/libraries/" .. SYSTEM_NAME .. "/",
	TURTLE_BRAINS_DIRECTORY .. "libraries/" .. SYSTEM_NAME .. "/",
	TURTLE_BRAINS_DIRECTORY .. "externals/libraries/" .. SYSTEM_NAME .. "/"
}

filter "configurations:debug"
	targetdir ("../build/" .. SYSTEM_NAME .. "/debug")
	objdir ("../build/" .. SYSTEM_NAME .. "/debug/objects")
	defines { "tb_debug_or_development_build", "tb_with_internal_tests", "DEBUG" }
	symbols ("On")
filter "configurations:development"
	targetdir ("../build/" .. SYSTEM_NAME .. "/development")
	objdir ("../build/" .. SYSTEM_NAME .. "/development/objects")
	defines { "tb_debug_or_development_build", "tb_without_splash_screen", "tb_with_debug_set", "tb_with_internal_tests", "NDEBUG" }
	symbols ("On")
	optimize ("On")
filter "configurations:release"
	targetdir ("../build/" .. SYSTEM_NAME .. "/release")
	objdir ("../build/" .. SYSTEM_NAME .. "/release/objects")
	defines { "tb_without_splash_screen", "tb_with_debug_set", "tb_with_internal_tests", "NDEBUG" }
	symbols ("On")
	optimize ("On")
filter "configurations:public"
	targetdir ("../build/" .. SYSTEM_NAME .. "/public")
	objdir ("../build/" .. SYSTEM_NAME .. "/public/objects")
	defines { "tb_without_debug_set", "NDEBUG" }
	symbols ("Off")
	optimize ("On")
filter {}

if (WINDOWS_SYSTEM_NAME == SYSTEM_NAME) then --------------------------------- Windows Platform Specifics (ALL projects)
	defines { "_CRT_SECURE_NO_WARNINGS", "_WINSOCK_DEPRECATED_NO_WARNINGS" }
	staticruntime "On"
	toolset "v143"
	characterset ("MBCS")
	buildoptions "/MP4"

	-- 2024-08-06: Removed this as I'm not sure it is strictly necessary for TrackBuilder, but if it is we should add
	--   the directory for all Windows projects to search out.
	-- libdirs {
	--  "C:/Program Files (x86)/Windows Kits/10/Lib/10.0.14393.0/um/x86/", --for dxguid
	-- 	"C:/Program Files (x86)/Windows Kits/8.1/Lib/winv6.3/um/x86/" --dxguid.lib
	-- }

elseif (MACOS_SYSTEM_NAME == SYSTEM_NAME) then --------------------------------- macOS Platform Specifics (ALL projects)
	externalincludedirs {
		"../source/",
		"../externals/includes/",
		TURTLE_BRAINS_DIRECTORY .. "source/",
		TURTLE_BRAINS_DIRECTORY .. "externals/includes/"
	}
elseif (LINUX_SYSTEM_NAME == SYSTEM_NAME) then --------------------------------- Linux Platform Specifics (ALL projects)
	includedirs {
		"/usr/includes/GL/"
	}
	libdirs {
		"/opt/lib/"
	}
elseif (WEB_SYSTEM_NAME == SYSTEM_NAME) then ------------------------ Web (Emscripten) Platform Specifics (ALL projects)
	-- 2024-08-06: May need define tb_without_networking again, but waiting until we make a web_build.
	defines { "tb_without_legacy_gl", "tb_without_threading" }
	linkoptions "-stdlib=libc++"
end

------------------------------------------------------------------------------------------------------------------------
------------------------------------------------------------------------------------------------------------------------
--     SPECIFIC PROJECTS BELOW!   SPECIFIC PROJECTS BELOW!    SPECIFIC PROJECTS BELOW!   SPECIFIC PROJECTS BELOW!     --
------------------------------------------------------------------------------------------------------------------------
------------------------------------------------------------------------------------------------------------------------

---------------------------------------------------------------------------------------------------------- asteroids
group "game"
project (PROJECT_NAME)
	kind ("WindowedApp")
	dependson { "turtle_brains" }
	debugargs { "--developer" }

	files {
		"../source/**.cpp",
		"../source/**.hpp",
		"../source/**.c",
		"../source/**.h"
	}
	excludes { "../**/doxygen/**" }

	if (WINDOWS_SYSTEM_NAME == SYSTEM_NAME) then
		entrypoint "mainCRTStartup"
		--links { "OpenGL32", "OpenAL32", "glew32", "libcurl", "crypt32", "ws2_32", "zlib" }
	elseif (MACOS_SYSTEM_NAME == SYSTEM_NAME) then
		files {
			"../source/**.mm",
			"./macos_info.plist",
			--"./macos_app.entitlements"
		}
		links { "AppKit.framework", "IOKit.framework", "OpenGL.framework", "OpenAL.framework", "GLEW", "curl" }
		xcodebuildsettings {
			-- paths here are  relative to the generated XCode project file.
			["INFOPLIST_FILE"] = "../../build/macos_info.plist",
			["CODE_SIGN_IDENTITY"] = "",
			["CODE_SIGNING_REQUIRED"] = "NO",
			["CODE_SIGNING_ALLOWED"] = "NO",
			["CODE_SIGN_ENTITLEMENTS"] = "" -- Also clear entitlements file
			--["CODE_SIGN_ENTITLEMENTS"] = ("../../build/macos_app.entitlements"),
		}
	elseif (LINUX_SYSTEM_NAME == SYSTEM_NAME) then
		linkoptions { "-Wl,--start-group" }
		links { "GL", "glew", "openal", "X11", "pthread", "curl", "tls", "ssl", "crypto" }
	end

	links { "turtle_brains_%{cfg.buildcfg}" }

	if (WEB_SYSTEM_NAME ~= SYSTEM_NAME) then
		postbuildcommands { "../scripts/post_build" .. SCRIPT_EXTENSION .. " --build-config %{cfg.buildcfg} --platform " .. TB_PLATFORM_DEFINE .. " --name %{prj.name}" }
	end

----------------------------------------------------------------------------- Visual Studio Solution or XCode Workspaces
if (WINDOWS_SYSTEM_NAME == SYSTEM_NAME or MACOS_SYSTEM_NAME == SYSTEM_NAME) then
	group "the_engine"
		externalproject "turtle_brains"
			location( TURTLE_BRAINS_DIRECTORY .. "build/" .. SYSTEM_NAME .. "/" )
			uuid "57940020-8E99-AEB6-271F-61E0F7F6B73B"
			kind "StaticLib"
			language "C++"
end
