@echo OFF

REM Windows MSBuild build script file
REM Usage:
REM "build" or "build all" - Execute both Debug and Release builds;
REM "build debug" - Execute Debug build;
REM "build release" - Execute Release build.

REM Important note:
REM This build script assumes that the path to the msbuild.exe file is provided.
REM However, this is not the case by default.  In order to add it to
REM default path list (PATH system variable on Windows), you need to open:
REM "Settings -> System -> Advanced system settings -> Environment Variables".
REM There would be User variables and System variables.  It is recommended
REM to add the path to User variables.  Click "Edit" button under User variables
REM section, and then add the path to the folder containing msbuild.exe file.
REM For example, for Visual Studio 2019 Community the path is:
REM "C:\Program Files (x86)\Microsoft Visual Studio\2019\Community\MSBuild\Current\Bin\".

REM Regarding copying:
REM We want all (not intermediate) build files to be in a single folder
REM because client and server executables need shared dll library within their folder.
REM But, we can't just build into a single folder since MSBuild and Visual Studio clean
REM all .pdb files, even if they are not assosiacted with building project.

REM "Delayed Expansion" means that the variable's value will be evaluated at execution-time,
REM not at parse-time -- that is exactly what we want! The usual %VARIABLE% expression
REM is a normal (parse-time) expansion; !VARIABLE! expression is a delayed (run-time) expansion.
REM This is needed for the special ERRORLEVEL system variable that contains command's exit code.
REM Of course we cannot know the result beforehand, that is why the Delayed Expansion is used.

REM Fail messages are stored in variables because the Delayed Expansion feature
REM consumes all exclamation points ('!') characters and it seems like this is
REM the only way around it -- to read fail messages through Delayed Expansion.

set build_debug=0
set build_release=0

set build_debug_fail_message=Failed to compile Debug build!
set build_release_fail_message=Failed to compile Release build!

set arg1=%~1

IF "%arg1%"=="" (

	set build_debug=1
	set build_release=1
	echo Build option argument is not provided, executing both Debug and Release builds.

) ELSE IF "%arg1%"=="all" (

	set build_debug=1
	set build_release=1
	echo Build option argument is set to "%~1", executing both Debug and Release builds.

) ELSE IF "%arg1%"=="release" (

	set build_release=1
	echo Build option argument is set to "%~1", executing Release build.

) ELSE IF "%arg1%"=="debug" (

	set build_debug=1
	echo Build option argument is set to "%~1", executing Debug build.

)

IF %build_debug%==1 (

	echo.
	echo --- Debug build ---
	echo.

	echo Step 1: Building a Visual Studio solution:

	SETLOCAL EnableDelayedExpansion
	(
		msbuild "qlight.sln" -nologo -property:Configuration=Debug -property:Platform=x64 -target:Build

		set build_debug_result=!ERRORLEVEL!
		IF !build_debug_result! NEQ 0 (
			echo.
			echo !build_debug_fail_message!
			echo.
			exit /b !build_debug_result!
		)
	)

	echo Step 2: Copying resource files into build folder:

	REM robocopy - a built-in Robust File Copy for Windows tool (apparently, it's been there since Windows Vista).
	REM /MIR - MIRror a directory tree: copy subdirectories, including empty ones
	REM            and delete destination files/directories that no longer exist in source.
	REM /NJH - No Job Header.
	REM /NJS - NO Job Summary.
	robocopy resources build\vs2019_qlight_Debug_x64\resources\ /MIR /NJH /NJS
	robocopy "%ASSIMP_PATH%"\bin\x64\ build\vs2019_qlight_Debug_x64\ assimp-vc143-mt.dll /NJH /NJS

)

IF %build_release%==1 (

	echo.
	echo --- Release build ---
	echo.

	echo Step 1: Building a Visual Studio solution:

	SETLOCAL EnableDelayedExpansion
	(
		msbuild "qlight.sln" -nologo -property:Configuration=Release -property:Platform=x64 -target:Build

		set build_release_result=!ERRORLEVEL!
		IF !build_release_result! NEQ 0 (
			echo.
			echo !build_release_fail_message!
			echo.
			exit /b !build_release_result!
		)
	)

	echo Step 2: Copying resource files into build folder:

	REM robocopy - a built-in Robust File Copy for Windows tool (apparently, it's been there since Windows Vista).
	REM /MIR - MIRror a directory tree: copy subdirectories, including empty ones
	REM            and delete destination files/directories that no longer exist in source.
	REM /NJH - No Job Header.
	REM /NJS - NO Job Summary.
	robocopy resources build\vs2019_qlight_Release_x64\resources\ /MIR /NJH /NJS
	robocopy "%ASSIMP_PATH%"\bin\x64\ build\vs2019_qlight_Release_x64\ assimp-vc143-mt.dll /NJH /NJS

)

echo.
echo All builds completed.
echo.