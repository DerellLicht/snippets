//**********************************************************************
//  wcmdline.cpp - Unicode command-line handling and console output
//  
//  Copyright (c) 2014-2024  Daniel D Miller
//  build: g++ -Wall -O2 -DUNICODE -D_UNICODE wcmdline.cpp -o wcmdline.exe
//**********************************************************************

#include <windows.h>
#include <stdio.h>
#include <tchar.h>
#include <limits.h>

//********************************************************************
int uprint(const TCHAR *fmt, ...)
{
   TCHAR consoleBuffer[3000] ;
   va_list al; //lint !e522

   va_start(al, fmt);   //lint !e1055 !e530
#ifdef UNICODE
   _vstprintf(consoleBuffer, fmt, al);   //lint !e64
   _tprintf(L"%s", consoleBuffer);
#else
   vsprintf(consoleBuffer, fmt, al);   //lint !e64
   printf("%s", consoleBuffer);
#endif   
   // OutputDebugString(consoleBuffer) ;
   va_end(al);
   return 1;
}

//*********************************************************************
static TCHAR **GetArgvCommandLine(int *argc)
{
#ifdef UNICODE
   LPTSTR argstr = GetCommandLineW();
   return CommandLineToArgvW(argstr, argc) ;
#else
   *argc = __argc;
   return  __argv;
#endif
}

//***********************************************************************
static void parse_command_line(void)
{
   // obtain command-line arguments in argv[] style array
   TCHAR **argv;
   int    argc;
   argv = GetArgvCommandLine(&argc);
   
   // home=C:\home\Anacom\Reference Docs\Supervisor 10
   TCHAR home[PATH_MAX+1];
   _wgetcwd(home, PATH_MAX);
   uprint(L"home=%s\n", home);
   
   // argc=2
   // arg 1: *.iss
   uprint(L"argc=%d\n", argc) ;
   int idx ;
   for (idx=1; idx<argc; idx++) {
      uprint(L"arg %d: %s\n", idx, argv[idx]) ;
   }
   
}  //lint !e550

//*********************************************************************
int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance,
   LPSTR lpszArgument, int nFunsterStil)
{
   parse_command_line();

   return (int) 0 ;
}  //lint !e715

