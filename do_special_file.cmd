::  This will be an example file that demonstrates flow-control options
::  To see An A-Z Index of Windows CMD commands, visit:
::  https://ss64.com/nt/
::  
::  This special script is used only for serial_enum.cpp

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
   clang-tidy --extra-arg=-isystemD:/tdm64/lib/gcc/x86_64-w64-mingw32/10.3.0/include/c++ --extra-arg=-isystemD:/tdm64/lib/gcc/x86_64-w64-mingw32/10.3.0/include/c++/x86_64-w64-mingw32 %2.cpp 
   @goto :eof

