::  This will be an example file that demonstrates flow-control options
::  To see An A-Z Index of Windows CMD commands, visit:
::  https://ss64.com/nt/

@if /I "%~2"=="" goto :usage
@if /I "%~1"=="--build" goto :build
@if /I "%~1"=="--check" goto :check

:usage
   @echo USAGE:
   @echo     do_one_file [--build] [--check] ^<filename^>
   @echo.
   @echo ARGUMENTS
   @echo    --build - run build command on ^<filename^>
   @echo    --check - run clang-tidy against ^<filename^>.cpp
   @echo.
   @echo    Either --build or --check are required
   @echo    ^<filename^>  [mandatory; filename does *not* include extension here]
   @echo.
   @echo    Example: 
   @echo    do_one_file --build read_files
   @goto :eof

:build
   g++ -Wall -O2 %2.cpp -o %2.exe
   @goto :eof

:check
   clang-tidy %2.cpp -- 
   @goto :eof

