//*****************************************************************
//*  qualify() modifies a string as required to generate a         
//*  "fully-qualified" filename, which is a filename that          
//*  complete drive specifier and path name.                       
//*                                                                
//*  input:  argptr: the input filename.                           
//*                                                                
//*  output: qresult, a bit-mapped unsigned int with the           
//*                   following definitions:                       
//*                                                                
//*          bit 0 == 1 if wildcards are present.                  
//*          bit 1 == 1 if no wildcards and path does not exist.   
//*          bit 2 == 1 if no wildcards and path exists as a file. 
//*          bit 7 == 1 if specified drive is invalid.             
//*                                                                
//*****************************************************************
//  try to modify this function to use string class, vs string.h
//*****************************************************************

#define  STANDALONE
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <direct.h>             //  _getdrive()
#include <sys/stat.h>
#include <ctype.h>              //  tolower()
#include <shlwapi.h>    // PathIsUNC(), etc
#include <limits.h>
#include <tchar.h>

// #include "qualify.h"
#define  QUAL_WILDCARDS    0x01
#define  QUAL_NO_PATH      0x02
#define  QUAL_IS_FILE      0x04
#define  QUAL_INV_DRIVE    0x80

#define  LOOP_FOREVER   true
#define  MAX_PATH_LEN      1024

static TCHAR path[MAX_PATH_LEN];

/******************************************************************/
unsigned qualify (TCHAR *argptr)
{
   TCHAR *pathptr = &path[0];
   TCHAR *strptr, *srchptr ;
   DWORD plen;
   int result ;
   struct _stat my_stat ;
   unsigned len, qresult = 0;

   if (_tcslen (argptr) == 0) {
      wcscpy(argptr, L".");
   }

   //  if arg len == 0 or arg is "."
   if (_tcscmp(argptr, _T(".")) == 0) {
      // printf("args=none or dot\n") ;         
      int drive = _getdrive ();     //  1 = A:
      //  see if we have a UNC drive...
      if (drive == 0) {
         GetCurrentDirectory (250, pathptr); //lint !e534
         _tcscat (pathptr, _T("\\*"));
         goto exit_point;
      }
   }
   //  05/26/25  These were shown unused, by clang-tidy
   //************************************************************
   //  determine requested drive number, in "A: = 1" format.
   //************************************************************
   //   else if arg == "x:"
//    else if (*(argptr + 1) == ':') { /*  a drive spec was provided  */
//       // printf("args=colon\n") ;      
//       TCHAR tempchar;
//       tempchar = *argptr;
//       drive = tolower (tempchar) - '`';   //  char - ('a' - 1)
//    }
//    //  else anything else
//    else {                       /*  a path with no drive spec was provided  */
//       // printf("args=no drive\n") ;      
//       drive = _getdrive ();     //  1 = A:
//    }

   //******************************************************
   //  strings in quotes will also foil the DOS routines;
   //  strip out all double quotes
   //******************************************************
   strptr = argptr;
   while (LOOP_FOREVER) {
      srchptr = _tcschr (strptr, '"');
      if (srchptr == 0)
         break;
      _tcscpy (srchptr, srchptr + 1);
      strptr = ++srchptr;
   }

   //******************************************************
   //  get expanded path (this doesn't support UNC)
   //******************************************************
   plen = GetFullPathName (argptr, PATH_MAX, (LPTSTR) pathptr, NULL);
   if (plen == 0)
      return QUAL_INV_DRIVE;

   len = _tcslen (pathptr);
   if (len == 3) {
      _tcscat (pathptr, _T("*"));
      qresult |= QUAL_WILDCARDS;
   }
   else {
      //  see if there are wildcards in argument.
      //  If not, see whether path is a directory or a file,
      //  or nothing.  If directory, append wildcard char
      if (_tcspbrk (pathptr, _T("*?")) == NULL) {
         if (*(pathptr + len - 1) == '\\') {
            len--;
            *(pathptr + len) = 0;
         }

         //  see what kind of file was requested
         //  FindFirstFile doesn't work with UNC paths,
         //  stat() doesn't either
         // handle = FindFirstFile (pathptr, &fffdata);
         if (PathIsUNC(pathptr)) {
            if (PathIsDirectory(pathptr)) {
               _tcscpy (pathptr + len, _T("\\*"));   //lint !e669  possible overrun
               qresult |= QUAL_WILDCARDS; //  wildcards are present.
            } else if (PathFileExists(pathptr)) {
               qresult |= QUAL_IS_FILE;   //  path exists as a normal file.
            } else {
               _tcscpy (pathptr + len, _T("\\*"));   //lint !e669  possible overrun
               qresult |= QUAL_WILDCARDS; //  wildcards are present.
            }
         } 
         //  process drive-oriented (non-UNC) paths
         else {
            result = _tstat(pathptr, &my_stat) ;
            if (result != 0) {
               qresult |= QUAL_INV_DRIVE; //  path does not exist.
            } else if (my_stat.st_mode & S_IFDIR) {
               _tcscpy (pathptr + len, _T("\\*"));   //lint !e669  possible overrun
               qresult |= QUAL_WILDCARDS; //  wildcards are present.
            } else {
               qresult |= QUAL_IS_FILE;   //  path exists as a normal file.
            }
         }

         // handle = FindFirstFile (pathptr, &fffdata);
         // if (handle == INVALID_HANDLE_VALUE)
         //   qresult |= QUAL_INV_DRIVE; //  path does not exist.
         // else {
         //   // if (fffdata.attrib & _A_SUBDIR)
         //   if (fffdata.dwFileAttributes & _A_SUBDIR) {
         //      _tcscpy (pathptr + len, "\\*");   //lint !e669  possible overrun
         //      qresult |= QUAL_WILDCARDS; //  wildcards are present.
         //   }
         //   else
         //      qresult |= QUAL_IS_FILE;   //  path exists as a normal file.
         // 
         //   FindClose (handle);
         // }
      }
   }

   //**********************************************
   //  copy string back to input, then return
   //**********************************************
 exit_point:
   _tcscpy (argptr, pathptr);
// printf("found: [%s]\n", pathptr) ;
// getchar() ;
   return (qresult); //lint !e438  drive
}

#ifdef  STANDALONE
//********************************************************************************
//  this solution is from:
//  https://github.com/coderforlife/mingw-unicode-main/
//********************************************************************************
#if defined(__GNUC__) && defined(_UNICODE)

#ifndef __MSVCRT__
#error Unicode main function requires linking to MSVCRT
#endif

#include <wchar.h>
#include <stdlib.h>

extern int _CRT_glob;
extern 
#ifdef __cplusplus
"C" 
#endif
void __wgetmainargs(int*,wchar_t***,wchar_t***,int,int*);

#ifdef MAIN_USE_ENVP
int wmain(int argc, wchar_t *argv[], wchar_t *envp[]);
#else
int wmain(int argc, wchar_t *argv[]);
#endif

int main() 
{
   wchar_t **enpv, **argv;
   int argc, si = 0;
   __wgetmainargs(&argc, &argv, &enpv, _CRT_glob, &si); // this also creates the global variable __wargv
#ifdef MAIN_USE_ENVP
   return wmain(argc, argv, enpv);
#else
   return wmain(argc, argv);
#endif
}

#endif //defined(__GNUC__) && defined(_UNICODE)

//**********************************************************************************
int wmain(int argc, wchar_t *argv[])
{
   wchar_t file_spec[MAX_PATH_LEN+1] = L"" ;
   
   for (int idx=1; idx<argc; idx++) {
      wchar_t *p = argv[idx] ;
      wcsncpy(file_spec, p, MAX_PATH_LEN);
      file_spec[MAX_PATH_LEN] = 0 ;
   }

   unsigned qresult = qualify(file_spec) ;
   if (qresult == QUAL_INV_DRIVE) {
      wprintf(L"%s: 0x%X\n", file_spec, qresult);
      return 1 ;
   }
   wprintf(L"input file spec: %s\n", file_spec);

   return 0;
}
#endif
