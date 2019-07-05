@ECHO OFF
REM - BEG: Run Windows CMD with locally-installed npm packages to be searched for execution first.
REM - =============================================================================================

SETLOCAL
  REM Set project full path. Do not use relative path in case of changing directory later.
  REM %CD% - pre-defined current directory variable does not work for 'Run as Administrator'.
  REM Use executable bat file path "%~dp0" or set full path to the project directly.
  SET PRJ_PATH=%~dp0%
  IF EXIST "%PRJ_PATH%" GOTO startCmd
  
  ECHO Project path doesn't exist: "%PRJ_PATH%"
  ECHO Please correct 'PRJ_PATH' in file: %0
  ECHO.
  PAUSE
  GOTO endNow
  
:startCmd
    REM - Set locally npm-install packages the first in PATH.
    SET PATH=%PRJ_PATH%\node_modules\.bin;%PATH%

    REM - START params: "<WINDOW_NAME>" /D "<WORKING_DIR>" <EXECUTABLE>
    START "image-processing" /D "%PRJ_PATH%" %ComSpec%

:endNow
ENDLOCAL

REM - =============================================================================================
REM - END: Run Windows CMD with locally-installed npm packages to be searched for execution first.
