//*************************************************************************
//  Copyright (c) 2013  Daniel D Miller
//  This program is freeware, with no restrictions on use, of any sort.
//  Finding Windows system directories
//  
//  Written by:  Daniel D Miller
//  
//  build: g++ -Wall -O2 -DUNICODE -D_UNICODE apptest.cpp -o apptest.exe
//*************************************************************************
//  Data on UNICODE printf() is from:
// http://blog.kalmbachnet.de/?postid=98
//*************************************************************************
//  Results:
// 
//  Windows 7 64-bit
// programs: [C:\Users\derelict\AppData\Roaming\Microsoft\Windows\Start Menu\Programs]
// personal: [C:\Users\derelict\Documents]
// appdata : [C:\Users\derelict\AppData\Roaming]
// common  : [C:\ProgramData\Microsoft\Windows\Start Menu\Programs]
// sysroot : [C:\Windows]
// system  : [C:\Windows\system32]
// 
//  Windows XP 32-bit
// programs: [C:\Documents and Settings\systest\Start Menu\Programs]
// personal: [C:\Documents and Settings\systest\My Documents]
// appdata : [C:\Documents and Settings\systest\Application Data]
// common  : [C:\Documents and Settings\All Users\Start Menu\Programs]
// sysroot : [C:\WINDOWS]
// system  : [C:\WINDOWS\system32]
//*************************************************************************

//  the elevated __MSVCRT_VERSION__ declaration is required in order to
//  enable _O_U16TEXT in fcntl.h
#define __MSVCRT_VERSION__    0x0800
#include <windows.h>
#include <stdio.h>
#include <io.h>
#include <fcntl.h>   //  _O_U16TEXT
#include <shlobj.h>  //  CSIDL_*
#include <tchar.h>

typedef unsigned int  uint ;

//**********************************************************************
void strip_newlines(char *rstr)
{
   int slen = (int) strlen(rstr) ;
   while (1) {
      if (slen == 0)
         break;
      if (*(rstr+slen-1) == '\n'  ||  *(rstr+slen-1) == '\r') {
         slen-- ;
         *(rstr+slen) = 0 ;
      } else {
         break;
      }
   }
}

//*************************************************************
//  each subsequent call to this function overwrites
//  the previous report.
//*************************************************************
char *get_system_message(void)
{
   static char msg[261] ;
   // int slen ;

   LPVOID lpMsgBuf;
   FormatMessageA(
      FORMAT_MESSAGE_ALLOCATE_BUFFER |
      FORMAT_MESSAGE_FROM_SYSTEM |
      FORMAT_MESSAGE_IGNORE_INSERTS,
      NULL,
      GetLastError(),
      MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
      (LPSTR) &lpMsgBuf,
      0, 0);
   // Process any inserts in lpMsgBuf.
   // ...
   // Display the string.
   strncpy(msg, (char *) lpMsgBuf, 260) ;

   // Free the buffer.
   LocalFree( lpMsgBuf );

   //  trim the newline off the message before copying it...
   strip_newlines(msg) ;
   return msg;
}

//**********************************************************************
typedef struct shgfp_s {
   int csidl_number ;
   TCHAR *desc ;
} shgfp_t ;

shgfp_t path_info[] = {
{ CSIDL_PROGRAMS,        L"programs" },
{ CSIDL_PERSONAL,        L"personal" },
{ CSIDL_APPDATA,         L"appdata" },
{ CSIDL_COMMON_PROGRAMS, L"common" },
{ CSIDL_WINDOWS,         L"sysroot" },
{ CSIDL_SYSTEM,          L"system" },
{ 0,                     L"end of list" }} ;

int main(void)
{
   TCHAR szPath[MAX_PATH];

   _setmode(_fileno(stdout), _O_U16TEXT); //  enable UNICODE printf()
   uint idx ;
   for (idx=0; path_info[idx].csidl_number != 0; idx++) {

      HRESULT result = SHGetFolderPath(NULL, 
                             path_info[idx].csidl_number,
                             NULL, 0, szPath) ;
      if (result == 0) {
         _tprintf(L"%-8s: [%s]\n", path_info[idx].desc, szPath) ;
      } else {
         _tprintf(L"%-8s: \n", path_info[idx].desc, get_system_message()) ;
      }
   }
   return 0;
}

