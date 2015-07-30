@setlocal enableextensions
@echo off

rem ===================================================================================

set "Curr_Path=%~dp0"
set "Curr_Path=%Curr_Path:~0,-1%"
set "PATH=%Curr_Path%;%PATH%"

rem ===================================================================================

set "Fonts_Lst=%Curr_Path%\%~n0.txt"
if not exist "%Fonts_Lst%" (goto :Quit_Script)

set "Fonts_Log=%Curr_Path%\%~n0.log"
set "Game_Path=%Curr_Path%\.."

if not "%~1" == "" (
	if exist "%Game_Path%\..\%~1" (set "Game_Path=%Game_Path%\..\%~1")
)
set "Fonts_Dir=%Game_Path%\gfx\vgui\fonts"

rem ===================================================================================

echo.>>"%Fonts_Log%"
echo>>"%Fonts_Log%" %date% [%time%] - Started...
echo.>>"%Fonts_Log%"

for /F "eol=; delims=	 tokens=1-5,* usebackq" %%a in ("%Fonts_Lst%") do (
	set "File_Name=%Fonts_Dir%\%%~a.tga"
	set "Font_Name=%%~b"
	set "Font_Style=%%~c"
	set "Font_Size=%%~d"
	set "Font_Chrs=%%~e"
	call :MakeTGAFont "File_Name" "Font_Name" "Font_Style" "Font_Size" "Font_Chrs"
)>>"%Fonts_Log%"

echo>>"%Fonts_Log%" %date% [%time%] - Finished.
echo.>>"%Fonts_Log%"

rem ===================================================================================

:Quit_Script

endlocal
exit /B 0

rem ===================================================================================

:MakeTGAFont

setlocal enableextensions

if "%~5" == "" (endlocal & exit /B 1) else (call set "Font_Chrs=%%%~5%%" )
if "%~4" == "" (endlocal & exit /B 1) else (call set "Font_Size=%%%~4%%" )
if "%~3" == "" (endlocal & exit /B 1) else (call set "Font_Style=%%%~3%%")
if "%~2" == "" (endlocal & exit /B 1) else (call set "Font_Name=%%%~2%%" )
if "%~1" == "" (endlocal & exit /B 1) else (call set "Targa_Name=%%%~1%%")

for %%f in ("%Targa_Name%") do (
	set "File_Path=%%~dpf"
	set "File_Name=%%~nxf"
)

set "File_Path=%File_Path:~0,-1%"
if not exist "%File_Path%\*" (md "%File_Path%")

set "FontStyle_B=false"
set "FontStyle_I=false"
set "FontStyle_U=false"
set "FontStyle_S=false"

for %%a in (%Font_Style%) do (
	if /I "%%~a" == "Bold"		(set "FontStyle_B=true") else (
	if /I "%%~a" == "Italic"	(set "FontStyle_I=true") else (
	if /I "%%~a" == "Underline"	(set "FontStyle_U=true") else (
	if /I "%%~a" == "Strikeout"	(set "FontStyle_S=true"))))
)

set "Cmd_Line=-font "%Font_Name%" -pointsize %Font_Size% -charset "%Font_Chrs%""
if /I "%FontStyle_B%" == "true" (set "Cmd_Line=%Cmd_Line% -bold"     )
if /I "%FontStyle_I%" == "true" (set "Cmd_Line=%Cmd_Line% -italic"   )
if /I "%FontStyle_U%" == "true" (set "Cmd_Line=%Cmd_Line% -underline")
if /I "%FontStyle_S%" == "true" (set "Cmd_Line=%Cmd_Line% -strikeout")

makevfont %Cmd_Line% "%Targa_Name%"
echo.

endlocal & exit /B 0

rem ===================================================================================
