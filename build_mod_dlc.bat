@echo off
setlocal EnableExtensions EnableDelayedExpansion

set "PluginName=Mods"
set "Config=Development"
set "ScriptDir=%~dp0"
if "%ScriptDir:~-1%"=="\" set "ScriptDir=%ScriptDir:~0,-1%"
set "OutputDir=%ScriptDir%\Saved\DLC"

rem Find .uproject and read EngineAssociation
set "ProjectFile="
for %%f in ("%ScriptDir%\*.uproject") do (
    set "ProjectFile=%%~ff"
    goto :FoundProject
)
:FoundProject
if not defined ProjectFile (
    echo [Error] .uproject file not found.
    exit /b 1
)

set "EngineVersion="
for /f "tokens=2 delims=:" %%a in ('findstr /I "EngineAssociation" "%ProjectFile%"') do set "EngineVersion=%%a"
if defined EngineVersion (
    set "EngineVersion=!EngineVersion:"=!"
    set "EngineVersion=!EngineVersion:,=!"
    for /f "tokens=1" %%v in ("!EngineVersion!") do set "EngineVersion=%%v"
)
if not defined EngineVersion (
    echo [Error] EngineAssociation not found.
    exit /b 1
)

rem Locate engine (HKCU Builds for Source/{GUID}/Installed, HKLM and LauncherInstalled.dat for Launcher, then Build.version fallback in HKCU Builds)
set "EngineDir="
for /f "tokens=2*" %%a in ('reg query "HKEY_CURRENT_USER\SOFTWARE\Epic Games\Unreal Engine\Builds" /v "%EngineVersion%" 2^>nul') do set "EngineDir=%%b"
if not defined EngineDir for /f "tokens=2*" %%a in ('reg query "HKEY_LOCAL_MACHINE\SOFTWARE\EpicGames\Unreal Engine\%EngineVersion%" /v "InstalledDirectory" 2^>nul') do set "EngineDir=%%b"
if not defined EngineDir if exist "%ProgramData%\Epic\UnrealEngineLauncher\LauncherInstalled.dat" (
    for /f "usebackq delims=" %%d in (`powershell -NoProfile -Command "(Get-Content '%ProgramData%\Epic\UnrealEngineLauncher\LauncherInstalled.dat' -Raw | ConvertFrom-Json).InstallationList | Where-Object { $_.AppName -eq 'UE_%EngineVersion%' } | Select-Object -First 1 -ExpandProperty InstallLocation" 2^>nul`) do set "EngineDir=%%d"
)
if not defined EngineDir (
    for /f "usebackq delims=" %%d in (`powershell -NoProfile -Command "$k = Get-ItemProperty 'HKCU:\SOFTWARE\Epic Games\Unreal Engine\Builds' -EA 0; if ($k) { $k.PSObject.Properties | Where-Object { $_.Name -notlike 'PS*' } | ForEach-Object { $f = Join-Path $_.Value 'Engine\Build\Build.version'; if (Test-Path $f) { $j = Get-Content $f -Raw | ConvertFrom-Json; $mm = [string]$j.MajorVersion + '.' + $j.MinorVersion; if ($mm -eq '%EngineVersion%' -or ($mm + '.' + $j.PatchVersion) -eq '%EngineVersion%') { $_.Value } } } | Select-Object -First 1 }" 2^>nul`) do set "EngineDir=%%d"
)
if not defined EngineDir (
    echo [Error] Engine "%EngineVersion%" not found.
    exit /b 1
)

set "EngineDir=!EngineDir:/=\!"
if "!EngineDir:~-1!"=="\" set "EngineDir=!EngineDir:~0,-1!"
set "RunUatBat=%EngineDir%\Engine\Build\BatchFiles\RunUAT.bat"
if not exist "%RunUatBat%" (
    echo [Error] RunUAT.bat not found at "%RunUatBat%".
    exit /b 1
)

rem Clear EngineDir so it does not override UnrealEngine.csproj.props $(EngineDir) in UE 5.8
set "EngineDir="

rem Build sources, cook DLC plugin only, stage text files via ModTextStaging, pak, and archive .pak
call "%RunUatBat%" BuildCookRun -ScriptsForProject="%ProjectFile%" -project="%ProjectFile%" -DLCName="%PluginName%" -BasedOnReleaseVersion=Dummy -NoP4 -platform=Win64 -clientConfig="%Config%" -build -cook -stage -pak -archive -archivedirectory="%OutputDir%" -VersionCookedContent -AdditionalCookerOptions="-DlcPluginOnly"
set "BuildErrorLevel=%errorlevel%"
if %BuildErrorLevel% neq 0 goto :Failed

echo.
echo DLC build successful: %OutputDir%\Windows\%PluginName%.pak
goto :End

:Failed
echo.
echo DLC build failed with code %BuildErrorLevel%.

:End
if /I not "%~1"=="-noPause" pause
exit /b %BuildErrorLevel%
