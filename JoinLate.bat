@echo off
setlocal

set "ENGINE=C:\Program Files\Epic Games\UE_5.8\Engine\Binaries\Win64\UnrealEditor.exe"

if not exist "%ENGINE%" (
    echo Could not find Unreal Engine 5.8 at:
    echo   %ENGINE%
    echo Open JoinLate.bat in Notepad and change the ENGINE line to point at UnrealEditor.exe.
    pause
    exit /b 1
)

set "PROJECT="
for %%F in ("%~dp0*.uproject") do set "PROJECT=%%~fF"

if not defined PROJECT (
    echo No .uproject file found next to JoinLate.bat.
    echo Move this file into the same folder as your .uproject and try again.
    pause
    exit /b 1
)

echo Starting a late joiner for:
echo   %PROJECT%
echo.
echo The window connects to the running session by itself.
echo If it loads the level alone instead, press the backtick key and type:
echo   open 127.0.0.1:17777

start "" "%ENGINE%" "%PROJECT%" 127.0.0.1:17777 -game -windowed -ResX=960 -ResY=540 -log
endlocal
