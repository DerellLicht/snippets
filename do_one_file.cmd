::  This will be an example file that demonstrates flow-control options
::  Here is a page with An A-Z Index of Windows CMD commands
::  https://ss64.com/nt/

@if /I "%~1"=="" goto :usage
@if /I "%~2"=="--build" goto :build
@if /I "%~2"=="--check" goto :check

:usage
   echo USAGE:
   echo    choices.cmd <filename> [--build] [--check]
   echo.
   echo ARGUMENTS
   echo       <filename>  [mandatory]
   echo       --build - run build command on <filename>
   echo       --check - run clang-tidy against <filename>.cpp
   echo.
   echo       Either --build or --check are required
   goto :eof

:build
   g++ -Wall -O2 %1.cpp -o %1.exe
   @goto :eof

:check
   clang-tidy %1.cpp -- 
   @goto :eof

