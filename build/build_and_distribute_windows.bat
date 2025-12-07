@ECHO off
setlocal enabledelayedexpansion

REM
REM Build and prepare the game for distribution. To actually upload/deploy use rhino and call the sister script like so:
REM   build_and_distribute.sh --windows
REM
REM <!-- Copyright (c) 2024 Tyre Bytes LLC - All Rights Reserved -->
REM --------------------------------------------------------------------------------------------------------------------

REM Grab the version string from file and if that failed ask the user.
SET /P BUILD_VERSION=<version.txt
IF NOT DEFINED BUILD_VERSION (
	SET /p BUILD_VERSION=version.txt not found, please enter the version number, [0.0.0]:
)

ECHO Creating a PUBLIC build of Asteroids v%BUILD_VERSION%
CALL make_project.bat --clean --build --public --build-version=%BUILD_VERSION%

REM 2025-01-12: There is VERY LIKELY always a false-positive here as I do not believe make_project.bat or sh returns a
REM   non-zero value when the build fails. So that is probably desired...
IF 0 == %errorlevel% (
	ECHO BuildAndDistribute: Creating a packaged zip to upload on itch.io and tyrebytes.com/games
	SET RUN_DIRECTORY="..\run"

	ECHO BuildAndDistribute: Package will be placed at: %RELEASE_DIRECTORY%

	FOR %%G IN (asteroids asteroids_steam asteroids_demo asteroids_demo_steam) DO (
		SET GAME_EXECUTABLE=%%G

		SET DEMO_SUFFIX=
		ECHO !GAME_EXECUTABLE! | FIND /I "demo" >NUL
		IF NOT ERRORLEVEL 1 (
			SET DEMO_SUFFIX=_demo
		)

		SET RELEASE_DIRECTORY="..\distribution\windows\!GAME_EXECUTABLE!_%BUILD_VERSION%_windows"

		IF EXIST "!RELEASE_DIRECTORY!" (RMDIR /Q /S "!RELEASE_DIRECTORY!")
		IF EXIST "!RELEASE_DIRECTORY!.zip" (DEL /Q /S /F "!RELEASE_DIRECTORY!.zip")

		ECHO BuildAndDistribute: Package will be placed at: !RELEASE_DIRECTORY!

		MKDIR !RELEASE_DIRECTORY!
		COPY "!RUN_DIRECTORY!\!GAME_EXECUTABLE!_public.exe" !RELEASE_DIRECTORY!\!GAME_EXECUTABLE!.exe
		COPY "!RUN_DIRECTORY!\glew32.dll" !RELEASE_DIRECTORY!
		COPY "!RUN_DIRECTORY!\steam_api64.dll" !RELEASE_DIRECTORY!
		COPY "!RUN_DIRECTORY!\release_notes.md" !RELEASE_DIRECTORY!
		COPY "!RUN_DIRECTORY!\readme.txt" !RELEASE_DIRECTORY!
		XCOPY /D /S /Q /EXCLUDE:exclude_windows.txt "!RUN_DIRECTORY!\data" "!RELEASE_DIRECTORY!\data\"

		powershell.exe -nologo -noprofile -command "& { Add-Type -A 'System.IO.Compression.FileSystem'; [IO.Compression.ZipFile]::CreateFromDirectory('!RELEASE_DIRECTORY!', '!RELEASE_DIRECTORY!.zip'); }"

		ECHO BuildAndDistribute: Build was a success, compressed to zip, now moving to rhino for further distribution.
		REM build_and_distribute_windows.bat does not actually perform any distributions it will just upload to
		REM the local linux file server (rhino) and await the build_and_distribution.sh to be run from there.

		IF NOT EXIST "Z:\game_builds\asteroids!DEMO_SUFFIX!\%BUILD_VERSION%\" (
			MKDIR "Z:\game_builds\asteroids!DEMO_SUFFIX!\%BUILD_VERSION%\"
		)

		COPY !RELEASE_DIRECTORY!.zip "Z:\game_builds\asteroids!DEMO_SUFFIX!\%BUILD_VERSION%\!GAME_EXECUTABLE!_%BUILD_VERSION%_windows.zip"
		ECHO Copied to Z:/game_builds, rhino/shelter/development/game_builds/ - Upload using build_and_distribute.sh from rhino.
	)
) ELSE (
	ECHO BuildAndDistribute: Build failed, skipping distribution.
)
