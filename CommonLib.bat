@echo off
setlocal enabledelayedexpansion

set "WORK=%~dp0"
set "PYTHON_VERSION_MINIMUM[0]=3"
set "PYTHON_VERSION_MINIMUM[1]=10"

py -V | find "Python" >NUL 2>NUL && (goto :PYTHON_FOUND)

:PYTHON_NOT_FOUND
echo Python %PYTHON_VERSION_MINIMUM[0]%.%PYTHON_VERSION_MINIMUM[1]% or later is required, and must be installed in PATH:
echo.
echo   https://www.python.org
echo.
echo If you have the correct version installed, ensure that it has the highest priorty in PATH.
goto :eof

:PYTHON_FOUND
for /f "tokens=2" %%a in ('py -V') do set "PYTHON_VERSION=%%a"

for /f "tokens=1,2 delims=." %%a in ("%PYTHON_VERSION%") do (
    set "PYTHON_VERSION[0]=%%a"
    set "PYTHON_VERSION[1]=%%b"
    set "PYTHON_VERSION=!PYTHON_VERSION[0]!.!PYTHON_VERSION[1]!"
)

if !PYTHON_VERSION[0]! GEQ !PYTHON_VERSION_MINIMUM[0]! (
    if !PYTHON_VERSION[1]! GEQ !PYTHON_VERSION_MINIMUM[1]! (
        goto :START
    ) else (
        goto :PYTHON_NOT_FOUND
    )
) else (
    goto :PYTHON_NOT_FOUND
)

:START
py -%PYTHON_VERSION% "%WORK%\tools\scripts\cli\main.py" %*

if %ERRORLEVEL% == -2 (
    set /p "PROMPT=Python dependencies are missing, install them now (Y/N)? "
    
    if /i "!PROMPT!" == "y" (
        echo.
        pip install --disable-pip-version-check --no-input -r "%WORK%\Requirements.txt"
        echo.
        goto :START
    )
)

exit /b %ERRORLEVEL%
