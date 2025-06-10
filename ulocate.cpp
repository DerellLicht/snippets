//****************************************************************************
//  Copyright (c) 2006-2021  Daniel D Miller
//  
//  ulocate: Locate filenames containing a certain component,
//  starting at specified path.
//                                                                 
//  Written by:   Daniel D. Miller  
//                                                                 
//  compile:  gcc -Wall -O2 -s ulocate.cpp -o ulocate
//  lint:    c:\lint9\lint-nt +v -width(160,4) -ic:\lint9 mingw.lnt -os(_lint.tmp) filename
//                                                                 
//****************************************************************************
// ver   date        description
// ====  ========    =========================================
// 1.00  02/04/02    derived from wlocate v1.02
// 1.01  03/02/07    Try to fix buffer handling
// 1.02  03/28/07    Add option to show current search directory
// 1.03  12/26/07    Add option to skip symbolic links
// 1.04  01/17/08    Add -p option to search the path
// 1.05  01/25/08    Add -w option for whole-word (exact) match
// 1.06  02/15/08    Allow stacked command-line switches
// 1.07  03/13/08    Create Win32 version of this utility
// 1.08  06/30/09    Fix long-standing bug in path search!
//                   Last element in path was dropped.
// 1.09  11/23/10    Add command-line switches to control output format
// 1.10  10/27/11    Remove printing of "file not found" error on unreadable files.
// 1.11  10/01/12    - Switching from stat() to _stati64() fixed the filesize-display 
//                     problem with files > 1GB in size.
//                   - Fix unintended processing of . and .. paths
//                   - Convert Win32 build to use Windows calls vs unix calls,
//                     to get rid of sporadic Windows system log warnings:
//                     "Invalid parameter passed to C runtime function."
// 1.12  06/29/16    Convert %llu references to inscrutable new C++ form 
// 1.13  07/13/17    Fix path handling for search in root directory
// 1.14  07/27/17    Add -/ switch to use backslash vs forward-slash in path
// 1.15  07/28/17    Add ULOCATE environment variable support
// 1.16  10/25/18    ULOCATE environment variable now only carries 
//                   optional search path.
// 1.17  10/29/18    ULOCATE mod - no, we *still* want to support switches,
//                   as *well* as base search path
// 1.18  08/27/21    -p option prepends '.' to search path
//****************************************************************************
//  Well, I've found the source of this inexplicable message in the Windows system log,
//  but I have no idea what the cause is.  Both errno and GetLastError()
//  indicate "no error".
// readdir enter   
// Invalid parameter passed to C runtime function. 
// readdir exit [0] [C:\Users\All Users\NVIDIA\Updatus\Download\33020A60/] 
// _stati64 enter [C:\Users\All Users\NVIDIA\Updatus\Download\33020A60/drsupdate.12601159_RUNASUSER.exe] 
// Invalid parameter passed to C runtime function. 
// _stati64 exit [C:\Users\All Users\NVIDIA\Updatus\Download\33020A60/drsupdate.12601159_RUNASUSER.exe]  
//  
//  I *suspect* that the basic issue is that I'm using unix-based functions
//  to traverse the directory tree, and am somewhere confusing Windows.
//****************************************************************************

char const * const Version = "ULOCATE.EXE, Version 1.18";

// #define  USE_NEW_LLU  1
#undef  USE_NEW_LLU

//lint -e537  Repeated include file
//lint -e451  header file repeatedly included but does not have a standard include guard

#ifdef __MINGW32__
#include <windows.h>
#ifdef  USE_NEW_LLU      
#define __STDC_FORMAT_MACROS
#include <inttypes.h>
#endif
#include <direct.h>  //  _getdrive()
#include <ctype.h>
#else
#include <string.h>
#endif
#include <stdio.h>
#include <unistd.h>  //  readlink() 
#include <stdlib.h>
#include <errno.h>
#include <time.h>
#include <limits.h>
#include <dirent.h>

#define  MAX_PATH_LEN   1024

//lint -e10    Expecting '}'

//lint -e534   Ignoring return value of function
//lint -e716   while(1) ... 
//lint -e818   Pointer parameter could be declared as pointing to const
//lint -e830   Location cited in prior message
//lint -e831   Reference cited in prior message

//lint -esym(526, __builtin_va_start)
//lint -esym(628, __builtin_va_start)
//lint -esym(551, follow_symlinks)

// typedef unsigned char       u8 ;
typedef unsigned int        uint ;
typedef unsigned long       u32 ;
typedef unsigned long long  u64 ;

// #ifndef __MINGW32__
static const bool LOOP_FOREVER = true ;
// #endif

static char target_path[MAX_PATH_LEN];
static char temp_path[MAX_PATH_LEN];

#ifdef __MINGW32__
union u64toul {
   ULONGLONG i ;
   u32 u[2] ;
};

//************************************************************
//  this struct is declared by Borland in IO.H, but is
//  not declared by MSC at all.
//  It is used to extract useful info from DOS file time/date
//************************************************************
struct  ftime   {
    unsigned    ft_tsec  : 5;   /* Two second interval */
    unsigned    ft_min   : 6;   /* Minutes */
    unsigned    ft_hour  : 5;   /* Hours */
    unsigned    ft_day   : 5;   /* Days */
    unsigned    ft_month : 4;   /* Months */
    unsigned    ft_year  : 7;   /* Year */
};

//*********************************************************
//  Convert date and time fields from file struct
//  into separate date and time elements.
//  This union requires io.h for struct ftime.
//*********************************************************
union parse_time {
   unsigned short dtime[2] ;
   struct ftime outdata ;
   } ;

#else
static char lfn_dest[MAX_PATH_LEN];
static char lfn_path[MAX_PATH_LEN];
static char res_dest[MAX_PATH_LEN];
#endif

static char name_comp[40];

static bool debug = false ;
static bool verbose = false ;
static bool path_shown = false ;
static bool follow_symlinks = false ;
static bool search_path = false ;
static bool whole_word_search = false ;
static bool use_backslashes = false ;

#define  OFMT_NONE   0x00
#define  OFMT_DATE   0x01
#define  OFMT_TIME   0x02
#define  OFMT_SIZES  0x04
#define  OFMT_SIZEL  0x08
#define  OFMT_NAME   0x10
static uint output_format = OFMT_NONE ;
static uint size_len = 6 ;

//**********************************************************
//  directory structure for directory_tree routines
//**********************************************************
struct dirs {
   dirs *brothers;
   dirs *sons;
   char *name;
};
static dirs *top = NULL;

//***************  function prototypes  ***************
static int read_dir_tree (dirs * cur_node);

//**********************************************************
#ifndef __MINGW32__
#ifndef bzero
void bzero(void *s, uint n)
{
   memset((char *) s, 0, n) ;
}
#endif
#endif

#ifdef __MINGW32__
//********************************************************************
//  On Windows platform, try to redefine printf/fprintf
//  so we can output code to a debug window.
//  Also, shadow syslog() within OutputDebugStringA()
//  Note: printf() remapping was unreliable,
//  but syslog worked great.
//********************************************************************
//lint -esym(714, syslog)
//lint -esym(759, syslog)
//lint -esym(765, syslog)
int syslog(const char *fmt, ...)
{
   char consoleBuffer[3000] ;
   va_list al; //lint !e522

   va_start(al, fmt);   //lint !e1055 !e530
   vsprintf(consoleBuffer, fmt, al);   //lint !e64
   // if (common_logging_enabled)
   //    fprintf(cmlogfd, "%s", consoleBuffer) ;
   OutputDebugStringA(consoleBuffer) ;
   va_end(al);
   return 1;
}
#endif

#ifdef __MINGW32__
/******************************************************************/
/*  qualify() modifies a string as required to generate a         */
/*  "fully-qualified" filename, which is a filename that          */
/*  complete drive specifier and path name.                       */
/*                                                                */
/*  input:  argptr: the input filename.                           */
/*                                                                */
/*  output: qresult, a bit-mapped unsigned int with the           */
/*                   following definitions:                       */
/*                                                                */
/*          bit 0 == 1 if wildcards are present.                  */
/*          bit 1 == 1 if no wildcards and path does not exist.   */
/*          bit 2 == 1 if no wildcards and path exists as a file. */
/*          bit 7 == 1 if specified drive is invalid.             */
/*                                                                */
/******************************************************************/
//****************************************************************************
// char *realpath(const char *path, char *resolved_path);
//  realpath  expands  all symbolic links and resolves references to 
//  '/./', '/../' and extra '/' characters in the null terminated string 
//  named by path and stores the canonicalized absolute pathname  in  the  
//  buffer of size MAX_PATH_LEN named by resolved_path.  
//  The resulting path will have no symbolic link, '/./' or '/../' components.
//****************************************************************************
// unsigned qualify (char *argptr)
char *realpath(const char *path, char *resolved_path)
{
   // static char path[_MAX_PATH];
   static WIN32_FIND_DATA fffdata;
   // char *pathptr = &path[0];
   char *pathptr = resolved_path ;
   const char *argptr = path ;
   // char *strptr, *srchptr ;
   DWORD plen;
   int drive;
   HANDLE handle;
   unsigned len ;
   // unsigned qresult = 0;

   if (path == NULL  ||  resolved_path == NULL) {
      errno = EINVAL ;
      return NULL;
   }

   //******************************************************
   //  first, determine requested drive number,            
   //  in "A: = 1" format.                                 
   //******************************************************
   if (strlen (argptr) == 0 || (strlen (argptr) == 1 && *argptr == '.')
      ) {                       /*  no arguments given  */
      // printf("args=none or dot\n") ;         
      drive = _getdrive ();     //  1 = A:
      //  see if we have a UNC drive...
      if (drive == 0) {
         GetCurrentDirectory (250, pathptr);
         // strcat (pathptr, "\\*");
         goto exit_point;
      }
   }
#ifndef __MINGW32__
   else if (*(argptr + 1) == ':') { /*  a drive spec was provided  */
      // printf("args=colon\n") ;      
      char tempchar = *argptr;
      drive = tolower (tempchar) - '`';   //  char - ('a' - 1)
   }
   else {                       /*  a path with no drive spec was provided  */
      // printf("args=no drive\n") ;      
      drive = _getdrive ();     //  1 = A:
   }
#endif

   //***********************************************************************
   //  strings in quotes will also foil the DOS routines;
   //  strip out all double quotes.
   //  For now, we won't worry about this; it needs to be done differently
   //  when emulating realpath(), since argptr is const.
   //***********************************************************************
   // strptr = argptr;
   // while (1) {
   //    srchptr = strchr (strptr, '"');
   //    if (srchptr == 0)
   //       break;
   //    strcpy (srchptr, srchptr + 1);
   //    strptr = ++srchptr;
   // }

   //******************************************************
   //  get expanded path (this doesn't support UNC)
   //******************************************************
   plen = GetFullPathName (argptr, _MAX_PATH, (LPTSTR) pathptr, NULL);
   if (plen == 0) {
      errno = ENOENT ;
      return NULL;
   }

   len = strlen (pathptr);
   if (len == 3) {
      // strcat (pathptr, "*");
      // qresult |= QUAL_WILDCARDS;
   }
   else {
      //  see if there are wildcards in argument.
      //  If not, see whether path is a directory or a file,
      //  or nothing.  If directory, append wildcard char
      if (strpbrk (pathptr, "*?") == NULL) {
         if (*(pathptr + len - 1) == '\\') {
            len--;
            *(pathptr + len) = 0;
         }

         //  see what kind of file was requested
         handle = FindFirstFile (pathptr, &fffdata);
         if (handle == INVALID_HANDLE_VALUE) {
            // qresult |= QUAL_INV_DRIVE; //  path does not exist.
            errno = ENOENT ;
            return NULL;
         }
         else {
            // if (fffdata.attrib & _A_SUBDIR)
            if (fffdata.dwFileAttributes & _A_SUBDIR) {
               // strcpy (pathptr + len, "\\*");   //lint !e669  possible overrun
               // qresult |= QUAL_WILDCARDS; //  wildcards are present.
            }
            // else
            //    qresult |= QUAL_IS_FILE;   //  path exists as a normal file.

            FindClose (handle);
         }
      }
   }

   //**********************************************
   //  copy string back to input, then return
   //**********************************************
 exit_point:
   // strcpy (argptr, pathptr);
   // return qresult ;
   return resolved_path ;
}  
#endif

//**********************************************************************
#ifndef __MINGW32__
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
#endif

//**********************************************************
char *mystrstr(char *haystack, char *needle)
{
   if (whole_word_search) {
      return (strcmp(needle, haystack) == 0) ? needle : 0 ;
   } 
   uint slen = strlen(needle) ;
   char *strptr = haystack ;
   while (1) {
      if (strncasecmp(strptr, needle, slen) == 0) {
         // printf("rlen=%d, strptr=%s, h
         return strptr ;
      }
      strptr++ ;
      // rlen++ ;
      if (*strptr == 0)
         return 0;
   }
}

//**********************************************************
void error_exit(int errcode, char *msg)
{
   if (errcode < 0)
       errcode = -errcode ;

   if (msg == 0) {
      printf("%s\n", strerror(errcode)) ;
   } else {
      printf("%s: %s\n", msg, strerror(errcode)) ;
   }
   exit(errcode) ;
}

//**********************************************************
//  allocate struct for dir listing                         
//  NOTE:  It is assumed that the caller will               
//         initialize the name[], ext[], attrib fields!!    
//**********************************************************
static dirs *new_dir_node (void)
{
   dirs *dtemp = new dirs;
   // if (dtemp == NULL)
   //    error_exit (ENOMEM, NULL);
   memset ((char *) dtemp, 0, sizeof (struct dirs));  //lint !e668
   return dtemp;
}

//**********************************************************
static void test_display_state(void)
{
   if (path_shown) {
      puts("") ;
      path_shown = false ;
   }
}

//**********************************************************
static void convert_slashes(char *pathname)
{
    char *p = pathname ;
    while (*p != 0)
    {
        if (*p == '/')
        {
            *p = '\\';
        }
        p++;
    }
}
                
//**********************************************************
static void print_output(int month, int day, long year, int hour, int mins, int secs, u64 fsize, char *pathname, bool is_dir)
{
   if (output_format & OFMT_DATE) {
      printf ("%02d/%02d/%02d ", month, day, (int) (year % 100)) ;
   }

   if (output_format & OFMT_TIME) {
      printf ("%02d:%02d:%02d ", hour, mins, secs) ;
   }

   if (output_format & OFMT_SIZES) {
      if (fsize > 99999999LU) {
#ifdef  USE_NEW_LLU      
         printf("%I64uM ", fsize / 1000000LU);
#else         
         printf("%5lluM ", fsize / 1000000LU);
#endif         
      }
      else if (fsize > 999999L) {
#ifdef  USE_NEW_LLU      
         printf("%5I64uK ", fsize / 1000L);
#else         
         printf("%5lluK ", fsize / 1000L);
#endif         
         // nputs (n.colorsize, tempstr);
         // nputs (n.colorsize ^ 0x08, "K");
      }
      else {
#ifdef  USE_NEW_LLU      
         printf("%6I64u ", fsize);
#else         
         printf("%6llu ", fsize);
#endif         
      }
   } else 
   if (output_format & OFMT_SIZEL) {
#ifdef  USE_NEW_LLU      
      printf("%*I64u ", (int) size_len, fsize);
#else         
      printf("%*llu ", (int) size_len, fsize);
#endif         
   }

   if (output_format & OFMT_NAME) {
      if (use_backslashes) {
         convert_slashes(pathname);
      }
      if (is_dir) 
         printf("[%s]", pathname) ;
      else
         printf("%s ", pathname) ;
   }
   
   // if (is_dir) {
   //    printf ("%02d/%02d/%02d %02d:%02d:%02d [%s]\n", 
   //       month, day, (int) (year % 100), hour, mins, secs, pathname);
   // } else {
   //    printf ("%02d/%02d/%02d %02d:%02d:%02d  %s", 
   //       month, day, (int) (year % 100), hour, mins, secs, pathname);
   // }
   puts("") ;
}

//**********************************************************
//  recursive routine to read directory tree
//  
//  The following warning:
//  Invalid parameter passed to C runtime function.
//  occurs for all files in directory:
//  c:\users\all users\nvidia\updatus\download\33020a60
//**********************************************************
static size_t prev_len = 0 ;
// static uint recurse_depth = 0 ;

static int read_dir_tree(dirs* cur_node)
{
   dirs *dtail = 0 ;
   dirs *dtemp ;
   char pathname[MAX_PATH_LEN] ;

   // recurse_depth++ ;
   if (debug)
      printf("entering [%s]\n", cur_node->name) ;
   // if (recurse_depth == 2)
   //    syslog("entering [%s]\n", cur_node->name) ;

   if (verbose) {
      printf("\r%-*s", (int) prev_len, cur_node->name) ;
      prev_len = strlen(cur_node->name) ;
      path_shown = true ;
   }

#ifdef __MINGW32__
   char dirname[MAX_PATH_LEN] ;
   FILETIME ft, lft;
   // struct tm *ftm ;
   u64toul iconv;
   parse_time outdt;
   int secs, mins, hour, day, month, year ;
   u64 fsize ;
   bool done = false;

   sprintf(dirname, "%s*", cur_node->name) ;
   if (debug)
      printf("dirname=[%s]\n", dirname) ;

   WIN32_FIND_DATA fdata ; //  long-filename file struct
   HANDLE handle = FindFirstFile (dirname, &fdata);
   //  according to MSDN, Jan 1999, the following is equivalent
   //  to the preceding... unfortunately, under Win98SE, it's not...
   // handle = FindFirstFileEx(target[i], FindExInfoStandard, &fdata, 
   //                      FindExSearchNameMatch, NULL, 0) ;
   if (handle == INVALID_HANDLE_VALUE)
      goto next_element;

   //  loop on find_next
   while (!done) {
      // if (n.show_all == 0) {
      //    if ((fdata.dwFileAttributes & 
      //       (FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_READONLY | FILE_ATTRIBUTE_SYSTEM)) != 0) {
      //       fn_okay = 0 ;
      //       goto search_next_file;
      //    }
      // }
      //  filter out directories if not requested
      bool fn_okay = true ;
      if (strcmp(fdata.cFileName, "..") == 0  ||  strcmp(fdata.cFileName, ".") == 0) {
         fn_okay = false;
      }
      if (debug) {
         printf("found %s, okay=%s\n", fdata.cFileName, (fn_okay) ? "true" : "false") ;
      }
      
      if (fn_okay) {
         wsprintf(pathname, "%s%s", cur_node->name, fdata.cFileName) ;
         
         //  process a directory entry
         if ((fdata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
            dtemp = new_dir_node() ;
            if (cur_node->sons == NULL) 
                cur_node->sons = dtemp ;
            else 
               dtail->brothers = dtemp ;  //lint !e644 !e613 NOLINT 
            dtail = dtemp ;   //  first time through this loop as directory, dtail gets assigned

            // dtail->name = new char[strlen(pathname)+1] ;
            dtail->name = new char[MAX_PATH_LEN] ;
            if (dtail->name == 0) {
               error_exit(ENOMEM, NULL) ; //  never returns
            }
            // sprintf(dtail->name, "%s/", fdata.cFileName) ;
            sprintf(dtail->name, "%s/", pathname) ;

            //  do what you wish with each located file here.
            //  Unfortunately, strstr() is case-sensitive...
            // if (strstr (dp->d_name, name_comp) != NULL) {
            if (mystrstr (fdata.cFileName, name_comp) != NULL) {
               // FILETIME ft ;
               // if (n.fdate_option == FDATE_LAST_ACCESS)
               //    ft = fdata.ftLastAccessTime;
               // else if (n.fdate_option == FDATE_CREATE_TIME)
               //    ft = fdata.ftCreationTime;
               // else
               //    ft = fdata.ftLastWriteTime;
               ft = fdata.ftLastWriteTime;
               FileTimeToLocalFileTime (&(ft), &lft);
               FileTimeToDosDateTime (&lft, &(outdt.dtime[1]), &(outdt.dtime[0]));
               secs = outdt.outdata.ft_tsec;
               mins = outdt.outdata.ft_min;
               hour = outdt.outdata.ft_hour;
               day  = outdt.outdata.ft_day;
               month = outdt.outdata.ft_month;
               year = 1980L + (long) outdt.outdata.ft_year;

               iconv.u[0] = fdata.nFileSizeLow;
               iconv.u[1] = fdata.nFileSizeHigh;
               fsize = iconv.i;

               test_display_state() ;
               print_output(month, day, year, hour, mins, secs, fsize, pathname, true) ;
               // printf ("%02d/%02d/%02d %02d:%02d:%02d [%s]\n", 
               //    month, day, (int) (year % 100), hour, mins, secs, pathname);
            }
         }
         //  process a file entry
         else {
            if (mystrstr (fdata.cFileName, name_comp) != NULL) {
               ft = fdata.ftLastWriteTime;
               FileTimeToLocalFileTime (&(ft), &lft);
               FileTimeToDosDateTime (&lft, &(outdt.dtime[1]), &(outdt.dtime[0]));
               secs = outdt.outdata.ft_tsec;
               mins = outdt.outdata.ft_min;
               hour = outdt.outdata.ft_hour;
               day  = outdt.outdata.ft_day;
               month = outdt.outdata.ft_month;
               year = 1980L + (long) outdt.outdata.ft_year;

               //  convert file size
               iconv.u[0] = fdata.nFileSizeLow;
               iconv.u[1] = fdata.nFileSizeHigh;
               fsize = iconv.i;

               test_display_state() ;
               print_output(month, day, year, hour, mins, secs, fsize, pathname, false) ;
               // printf ("%02d/%02d/%02d %02d:%02d:%02d  %s\n", 
               //    month, day, (int) (year % 100), hour, mins, secs, pathname);
            }
         }
      }  //  if file is parseable...

// search_next_file:
      //  search for another file
      if (FindNextFile (handle, &fdata) == 0)
         done = true;
   }  //  while not done

   FindClose (handle);
#else
   struct stat statbuf ; 
   struct dirent *dp;

   // printf("search %s\n", dir_path) ;
   DIR *dirp = opendir(cur_node->name);
   if (dirp == NULL) {
      // error_exit(errno, NULL) ; //  never returns
      goto next_element;
      // perror(dir_path);
      // exit(1) ;
   }

   //  loop on find_next
   struct tm *ftm ;
   int secs, mins, hour, day, month ;
   long year ;
   u64 fsize ;
   while (LOOP_FOREVER) {
// syslog("readdir enter \n") ;
      dp = readdir(dirp) ;
// syslog("readdir exit [%u] [%s]\n", (uint) GetLastError(), cur_node->name) ;
      if (dp == NULL)
         break;
      if (strcmp(dp->d_name, "..") == 0  ||  strcmp(dp->d_name, ".") == 0) 
         continue;
      sprintf(pathname, "%s%s", cur_node->name, dp->d_name) ;
      if (debug)
         printf("pathname=[%s]\n", pathname) ;

// #ifdef __MINGW32__
//       //  switching from stat() to _stati64() fixed the filesize-display problem
//       //  with files > 1GB in size.
//       // if (stat(pathname, &statbuf) != 0) {
//       int iresult = _stati64(pathname, &statbuf) ;
//       if (iresult != 0) {
//          test_display_state() ;
//          if (errno != ENOENT) {
//             printf("stat: %s: [%u] %s\n", pathname, errno, strerror(errno)) ;
//          }
//          continue;
//       }
// #else
      if (lstat(pathname, &statbuf) != 0) {
         test_display_state() ;
         printf("lstat: %s: %s\n", pathname, strerror(errno)) ;
         continue;
      }

      //  The following flags are defined for the st_mode field:
      // 
      //  S_IFMT     0170000   bitmask for the file type bitfields
      //  S_IFSOCK   0140000   socket
      //  S_IFLNK    0120000   symbolic link
      //  S_IFREG    0100000   regular file
      //  S_IFBLK    0060000   block device
      //  S_IFDIR    0040000   directory
      //  S_IFCHR    0020000   character device
      //  S_IFIFO    0010000   fifo
      //  S_ISUID    0004000   set UID bit
      //  S_ISGID    0002000   set GID bit (see below)
      //  S_ISVTX    0001000   sticky bit (see below)
      //  S_IRWXU      00700   mask for file owner permissions
      //  S_IRUSR      00400   owner has read permission
      //  S_IWUSR      00200   owner has write permission
      //  S_IXUSR      00100   owner has execute permission
      //  S_IRWXG      00070   mask for group permissions
      //  S_IRGRP      00040   group has read permission
      //  S_IWGRP      00020   group has write permission
      //  S_IXGRP      00010   group has execute permission
      //  S_IRWXO      00007   mask for permissions for others (not in group)
      //  S_IROTH      00004   others have read permission
      //  S_IWOTH      00002   others have write permisson
      //  S_IXOTH      00001   others have execute permission
      typedef struct fattr_bits_s {
         unsigned perms  :  9 ;
         unsigned setids :  3 ;
         unsigned types  :  4 ;
         unsigned unused : 16 ;
      } fattr_bits_t ;
      typedef union fattribs_s {
         fattr_bits_t fb ;
         unsigned raw ;
      } fattribs_t ;
      fattribs_t fattr ;

      int linked = 0 ;
      fattr.raw = statbuf.st_mode ;
      // printf("%03o ", fattr.fb.types) ;
      //  data in pathname
      if (follow_symlinks  &&  fattr.fb.types == 012) {
         //  note: readlink requires full path and filename...
         bzero(lfn_dest, sizeof(lfn_dest)) ;
         if (readlink(pathname, lfn_dest, sizeof(lfn_dest)) > 0) {
            if (debug)
               printf("lfn_dest=[%s]\n", lfn_dest) ;
            //  data in lfn_dest
            // printf("[%s -> %s]\n", pathname, lfn_dest) ;
            // int rllen = strlen(lfn_dest) ;
            sprintf(lfn_path, "%s%s", cur_node->name, lfn_dest) ;
            //  data in lfn_path
            char *p = realpath(lfn_path, res_dest) ;
            //  data in res_dest
            if (p == 0) {
               if (debug) {
                  test_display_state() ;
                  printf("realpath: %s: %s\n", lfn_dest, strerror(errno)) ;
               }
               continue;
            }
            if (debug)
               printf("lfn_path=[%s]\n", lfn_path) ;
            if (lstat(res_dest, &statbuf) != 0) {
               test_display_state() ;
               printf("lstat(link): %s: %s\n", res_dest, strerror(errno)) ;
               continue;
            }
            // printf("%s => %s\n", dp->d_name, res_dest) ;
            strcpy(pathname, res_dest) ;
            if (debug)
               printf("pathname(link)=[%s]\n", pathname) ;
            //  data in pathname
            // strcpy(dp->d_name, res_dest) ;
            linked = 1 ;
            // ftemp->linktgt = (char *) new char[sizeof(lfn_dest)] ;
            // if (ftemp->linktgt != 0) {
            //    bzero(ftemp->linktgt, rllen+1) ;
            //    strcpy(ftemp->linktgt, lfn_dest) ;
            //    // printf("{%s -> %s}\n", ftemp->filename, ftemp->linktgt) ;
            // }
         } else {
            test_display_state() ;
            perror(lfn_dest) ;
            continue;
         }
      }
      fsize = (u64) statbuf.st_size ;

      //  process directory
      if ((S_ISDIR(statbuf.st_mode)) != 0) {
         dtemp = new_dir_node() ;
         if (cur_node->sons == NULL) 
             cur_node->sons = dtemp ;
         else 
            dtail->brothers = dtemp ;  //lint !e644
         dtail = dtemp ;

         // dtail->name = new char[strlen(pathname)+1] ;
         dtail->name = new char[MAX_PATH_LEN] ;
         if (dtail->name == 0) {
            error_exit(ENOMEM, NULL) ; //  never returns
         }
         sprintf(dtail->name, "%s/", pathname) ;
         // printf("... [%s]\n", dtail->name) ;
         // dtail->attrib = (u8) statbuf.st_mode ;
         if (linked)
            continue;

         //  do what you wish with each located file here.
         //  Unfortunately, strstr() is case-sensitive...
         // if (strstr (dp->d_name, name_comp) != NULL) {
         if (mystrstr (dp->d_name, name_comp) != NULL) {
            // printf ("[%s%s]\n", show_path, dp->d_name);
            // FileTimeToLocalFileTime(&(fdata.ftLastWriteTime), &lft) ;
            // FileTimeToSystemTime(&lft, &stm) ;
            // time_t statbuf.st_atime;    /* time of last access */
            // time_t statbuf.st_mtime;    /* time of last modification */
            // time_t statbuf.st_ctime;    /* time of last status change */
            ftm = localtime((time_t *) &statbuf.st_mtime) ;
            secs  = ftm->tm_sec ;
            mins  = ftm->tm_min ;
            hour  = ftm->tm_hour ;
            day   = ftm->tm_mday ;
            month = ftm->tm_mon + 1 ;
            year = 1900 + ftm->tm_year ;

            test_display_state() ;
            print_output(month, day, year, hour, mins, secs, fsize, pathname, true) ;
            // printf ("%02d/%02d/%02d %02d:%02d:%02d [%s]\n", 
            //    month,
            //    day,
            //    (int) (year % 100),
            //    hour,
            //    mins,
            //    secs,
            //    pathname);
         }
      }

      //  we found a normal file
      else {
         if (mystrstr (dp->d_name, name_comp) != NULL) {
            // time_t statbuf.st_atime;    /* time of last access */
            // time_t statbuf.st_mtime;    /* time of last modification */
            // time_t statbuf.st_ctime;    /* time of last status change */
            ftm = localtime((time_t *) &statbuf.st_mtime) ;
            secs  = ftm->tm_sec ;
            mins  = ftm->tm_min ;
            hour  = ftm->tm_hour ;
            day   = ftm->tm_mday ;
            month = ftm->tm_mon + 1 ;
            year = 1900 + ftm->tm_year ;

            //  convert file size
            // u64toul iconv;
            // iconv.u[0] = fdata.nFileSizeLow;
            // iconv.u[1] = fdata.nFileSizeHigh;
            // ftemp->fsize = iconv.i;
            
            test_display_state() ;
            print_output(month, day, year, hour, mins, secs, fsize, pathname, false) ;
            // printf ("%02d/%02d/%02d %02d:%02d:%02d  %s\n", 
            //    month,
            //    day,
            //    (int) (year % 100),
            //    hour,
            //    mins,
            //    secs,
            //    pathname);
         }
      }
   }

   // FindClose(handle) ;
   closedir(dirp) ;
#endif

next_element:
   //  next, build tree lists for subsequent levels (recursive)
   dirs* ktemp ;
   for (ktemp = cur_node->sons; ktemp != 0; ktemp = ktemp->brothers) 
      read_dir_tree(ktemp) ;

   // if (recurse_depth == 2)
   //    syslog("leaving [%s]\n", cur_node->name) ;
   // recurse_depth-- ;
   return 0;
}

//**********************************************************
static int build_dir_tree(char* tpath)
{
   char* strptr ;
   // char base_path[MAX_PATH_LEN];

   // strcpy(base_path, tpath) ;
   //  allocate struct for dir listing
   top = new_dir_node() ;

   //  derive root path name
   strptr = (char *) new char[MAX_PATH_LEN+1] ;
   // if (strptr == 0) 
   //    error_exit(ENOMEM, NULL) ;
   top->name = strptr ;
   strncpy(top->name, tpath, MAX_PATH_LEN) ;

   //  make sure base path ends with a slash
   int base_len = strlen(top->name) ;  //lint !e713
   if (top->name[base_len-1] != '/') {
      strcat(top->name, "/") ;   // NOLINT
      // base_len++ ;
   }
   // strcpy(dir_path, base_path) ;

   //  call the recursive function
   read_dir_tree(top) ;

   return 0;
}

//**********************************************************
static int read_single_dir(char *sub_path)
{
   //  why not use cur_node->name instead of dir_path??
   if (debug)
      printf("entering [%s]\n", sub_path) ;

#ifdef __MINGW32__
   // unsigned cut_dot_dirs ;
   char dirname[MAX_PATH_LEN] ;
   char pathname[MAX_PATH_LEN] ;
   FILETIME ft, lft;
   u64toul iconv;
   parse_time outdt;
   int secs, mins, hour, day, month, year ;
   u64 fsize ;
   bool done = false;

// entering [c:\utility]
// dirname=[c:\utility*]
// found utility, okay=true
   sprintf(dirname, "%s\\*", sub_path) ;
   if (debug)
      printf("dirname=[%s]\n", dirname) ;

   WIN32_FIND_DATA fdata ; //  long-filename file struct
   HANDLE handle = FindFirstFile (dirname, &fdata);
   //  according to MSDN, Jan 1999, the following is equivalent
   //  to the preceding... unfortunately, under Win98SE, it's not...
   // handle = FindFirstFileEx(target[i], FindExInfoStandard, &fdata, 
   //                      FindExSearchNameMatch, NULL, 0) ;
   if (handle == INVALID_HANDLE_VALUE)
      goto next_element;

   //  loop on find_next
   while (!done) {
      // if (n.show_all == 0) {
      //    if ((fdata.dwFileAttributes & 
      //       (FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_READONLY | FILE_ATTRIBUTE_SYSTEM)) != 0) {
      //       fn_okay = 0 ;
      //       goto search_next_file;
      //    }
      // }
      //  filter out directories if not requested
      bool fn_okay = true ;
      if (strcmp(fdata.cFileName, "..") == 0  ||  strcmp(fdata.cFileName, ".") == 0) 
         fn_okay = false;
      if (debug)
         printf("found %s, okay=%s\n", fdata.cFileName,
            (fn_okay) ? "true" : "false") ;
      
      if (fn_okay) {
         wsprintf(pathname, "%s\\%s", sub_path, fdata.cFileName) ;
         if (debug)
            printf("pathname=[%s]\n", pathname) ;
         
         //  process a directory entry
         if ((fdata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
            // dtemp = new_dir_node() ;
            // if (cur_node->sons == NULL) 
            //     cur_node->sons = dtemp ;
            // else 
            //    dtail->brothers = dtemp ;  //lint !e644 !e613
            // dtail = dtemp ;

            // dtail->name = new char[strlen(pathname)+1] ;
            // dtail->name = new char[MAX_PATH_LEN] ;
            // if (dtail->name == 0) {
            //    error_exit(ENOMEM, NULL) ; //  never returns
            // }
            // sprintf(dtail->name, "%s/", fdata.cFileName) ;
            // sprintf(dtail->name, "%s/", pathname) ;

            //  do what you wish with each located file here.
            //  Unfortunately, strstr() is case-sensitive...
            // if (strstr (dp->d_name, name_comp) != NULL) {
            if (mystrstr (fdata.cFileName, name_comp) != NULL) {
               // ftemp->attrib = (uchar) fdata.dwFileAttributes;
               
               // printf ("[%s%s]\n", show_path, dp->d_name);
               // time_t statbuf.st_atime;    /* time of last access */
               // time_t statbuf.st_mtime;    /* time of last modification */
               // time_t statbuf.st_ctime;    /* time of last status change */
               // ftm = localtime((time_t *) &statbuf.st_mtime) ;
               // secs  = ftm->tm_sec ;
               // mins  = ftm->tm_min ;
               // hour  = ftm->tm_hour ;
               // day   = ftm->tm_mday ;
               // month = ftm->tm_mon + 1 ;
               // year = 1900 + ftm->tm_year ;

               // FILETIME ft ;
               // if (n.fdate_option == FDATE_LAST_ACCESS)
               //    ft = fdata.ftLastAccessTime;
               // else if (n.fdate_option == FDATE_CREATE_TIME)
               //    ft = fdata.ftCreationTime;
               // else
               //    ft = fdata.ftLastWriteTime;
               ft = fdata.ftLastWriteTime;
               FileTimeToLocalFileTime (&(ft), &lft);
               FileTimeToDosDateTime (&lft, &(outdt.dtime[1]), &(outdt.dtime[0]));
               secs = outdt.outdata.ft_tsec;
               mins = outdt.outdata.ft_min;
               hour = outdt.outdata.ft_hour;
               day  = outdt.outdata.ft_day;
               month = outdt.outdata.ft_month;
               year = 1980L + (long) outdt.outdata.ft_year;

               iconv.u[0] = fdata.nFileSizeLow;
               iconv.u[1] = fdata.nFileSizeHigh;
               fsize = iconv.i;

               test_display_state() ;
               print_output(month, day, year, hour, mins, secs, fsize, pathname, true) ;
               // printf ("%02d/%02d/%02d %02d:%02d:%02d [%s]\n", 
               //    month,
               //    day,
               //    (int) (year % 100),
               //    hour,
               //    mins,
               //    secs,
               //    pathname);
            }
         }
         //  process a file entry
         else {
            if (mystrstr (fdata.cFileName, name_comp) != NULL) {
               // time_t statbuf.st_atime;    /* time of last access */
               // time_t statbuf.st_mtime;    /* time of last modification */
               // time_t statbuf.st_ctime;    /* time of last status change */
               // ftm = localtime((time_t *) &statbuf.st_mtime) ;
               // secs  = ftm->tm_sec ;
               // mins  = ftm->tm_min ;
               // hour  = ftm->tm_hour ;
               // day   = ftm->tm_mday ;
               // month = ftm->tm_mon + 1 ;
               // year = 1900 + ftm->tm_year ;
               ft = fdata.ftLastWriteTime;
               FileTimeToLocalFileTime (&(ft), &lft);
               FileTimeToDosDateTime (&lft, &(outdt.dtime[1]), &(outdt.dtime[0]));
               secs  = outdt.outdata.ft_tsec;
               mins  = outdt.outdata.ft_min;
               hour  = outdt.outdata.ft_hour;
               day   = outdt.outdata.ft_day;
               month = outdt.outdata.ft_month;
               year  = 1980L + (long) outdt.outdata.ft_year;

               //  convert file size
               iconv.u[0] = fdata.nFileSizeLow;
               iconv.u[1] = fdata.nFileSizeHigh;
               fsize = iconv.i;

               test_display_state() ;
               print_output(month, day, year, hour, mins, secs, fsize, pathname, false) ;
               // printf ("%02d/%02d/%02d %02d:%02d:%02d  %s\n", 
               //    month, day, (int) (year % 100), hour, mins, secs, pathname);
            }
         }
      }  //  if file is parseable...

// search_next_file:
      //  search for another file
      if (FindNextFile (handle, &fdata) == 0)
         done = true;
   }  //  while not done

   FindClose (handle);
next_element:
#else

   unsigned cut_dot_dirs ;
   DIR *dirp;                   /* pointer to directory */
   struct dirent *dp;
   struct stat statbuf ; 
   char pathname[MAX_PATH_LEN] ;

   //  why not use cur_node->name instead of dir_path??
   if (debug)
      printf("entering [%s]\n", sub_path) ;

   // printf("search %s\n", dir_path) ;
   dirp = opendir(sub_path);
   if (dirp == NULL) {
      if (verbose)
         printf("%s: %s\n", sub_path, strerror(errno)) ;
      return (int) errno;
   }

   //  loop on find_next
   struct tm *ftm ;
   int secs, mins, hour, day, month ;
   long year ;
   u64 fsize ;
   while ((dp = readdir(dirp)) != NULL) {
      sprintf(pathname, "%s/%s", sub_path, dp->d_name) ;
      if (debug)
         printf("pathname=[%s]\n", pathname) ;
      if (lstat(pathname, &statbuf) != 0) {
         test_display_state() ;
         printf("lstat: %s: %s\n", pathname, strerror(errno)) ;
         continue;
      }
      fsize = (u64) statbuf.st_size ;

      //  process directory
      if ((S_ISDIR(statbuf.st_mode)) != 0) {
         //  skip '.' and '..', but NOT .ncftp (for example)
         if (dp->d_name[0] != '.')  
            cut_dot_dirs = 0 ;
         else if (dp->d_name[1] == 0)
            cut_dot_dirs = 1 ;
         else if (dp->d_name[1] == '.'  &&  dp->d_name[2] == 0)
            cut_dot_dirs = 1 ;
         else 
            cut_dot_dirs = 0 ;

         if (cut_dot_dirs)
            continue;

         //  do what you wish with each located file here.
         //  Unfortunately, strstr() is case-sensitive...
         // if (strstr (dp->d_name, name_comp) != NULL) {
         if (mystrstr (dp->d_name, name_comp) != NULL) {
            // printf ("[%s%s]\n", show_path, dp->d_name);
            // FileTimeToLocalFileTime(&(fdata.ftLastWriteTime), &lft) ;
            // FileTimeToSystemTime(&lft, &stm) ;
            // time_t statbuf.st_atime;    /* time of last access */
            // time_t statbuf.st_mtime;    /* time of last modification */
            // time_t statbuf.st_ctime;    /* time of last status change */
            ftm = localtime((time_t *) &statbuf.st_mtime) ;
            secs  = ftm->tm_sec ;
            mins  = ftm->tm_min ;
            hour  = ftm->tm_hour ;
            day   = ftm->tm_mday ;
            month = ftm->tm_mon + 1 ;
            year = 1900 + ftm->tm_year ;

            test_display_state() ;
            print_output(month, day, year, hour, mins, secs, fsize, pathname, true) ;
            // printf ("%02d/%02d/%02d %02d:%02d:%02d [%s]\n", 
            //    month,
            //    day,
            //    (int) (year % 100),
            //    hour,
            //    mins,
            //    secs,
            //    pathname);
         }
      }

      //  we found a normal file
      else {
         if (mystrstr (dp->d_name, name_comp) != NULL) {
            // time_t statbuf.st_atime;    /* time of last access */
            // time_t statbuf.st_mtime;    /* time of last modification */
            // time_t statbuf.st_ctime;    /* time of last status change */
            ftm = localtime((time_t *) &statbuf.st_mtime) ;
            secs  = ftm->tm_sec ;
            mins  = ftm->tm_min ;
            hour  = ftm->tm_hour ;
            day   = ftm->tm_mday ;
            month = ftm->tm_mon + 1 ;
            year = 1900 + ftm->tm_year ;
            
            test_display_state() ;
            print_output(month, day, year, hour, mins, secs, fsize, pathname, false) ;
            // printf ("%02d/%02d/%02d %02d:%02d:%02d  %s\n", 
            //    month,
            //    day,
            //    (int) (year % 100),
            //    hour,
            //    mins,
            //    secs,
            //    pathname);
         }
      }
   }

   // FindClose(handle) ;
   closedir(dirp) ;
#endif

   return 0;
}

//************************************************************
#ifdef __MINGW32__
static const char path_sep = ';' ;
#else
static const char path_sep = ':' ;
#endif

static int search_path_for_name(void)
{
   char mypath[MAX_PATH_LEN+1] ;
   char *my_path = getenv("PATH") ;
   if (my_path == 0) {
      FILE *fd = popen("echo $PATH", "r") ;
      if (fd == 0) {
         perror("popen(PATH)") ;
         return ENODEV;
      }
      my_path = fgets(mypath, sizeof(mypath), fd) ;
      if (my_path == 0) {
         perror("fgets(PATH)") ;
         return ENODEV;
      }
      pclose(fd) ;
   } 
#ifndef __MINGW32__
   strip_newlines(my_path);
#endif
   //  in Windows environment, prepend '.' to path
#ifdef __MINGW32__
   {
   char tempath[MAX_PATH_LEN+1] ;
   sprintf(tempath, ".;%s", my_path);  // add current directory to path
   strncpy(mypath, tempath, MAX_PATH_LEN);
   my_path = mypath ;
   }
#endif
   if (debug) {
      printf("PATH=[%s]\n", my_path) ;
   }
   char *hd = my_path ;
   while (1) {
      unsigned copy_count = 0 ;
      char *tptr = temp_path ;
      while (*hd != path_sep  &&  *hd != 0) {
         *tptr++ = *hd++ ;
         copy_count++ ;
      }
      *tptr = 0 ; //  NULL-term the string
      if (*hd == 0  &&  copy_count == 0) {
         break;
      }
      if (*hd != 0) 
         hd++ ;   //  skip the colon
      read_single_dir(temp_path) ;
      if (*hd == 0)
         break;
   }
   return 0;
}

//************************************************************
void usage (void)
{
   puts("Usage: ulocate [options] name_component start_path");
   puts("This program displays all filenames containing a specified");
   puts("name component, starting at directory [start_path].");
   puts("Default start_path is current location.") ;
   puts("") ;
   puts("output-format options:") ;
   puts(" -d  show file date") ;
   puts(" -t  show file time") ;
   puts(" -s  show file size (short format)") ;
   puts(" -S  show file size (byte format)") ;
   puts("     -xl  l = width of size field for -S (default = 6)") ;
   puts(" -n  show file name (default)") ;
   puts("") ;
   puts("other options:") ;
   puts(" -v  Verbose mode - show search paths") ;
   puts(" -b  Debug mode - show other process information") ;
   puts(" -l  follow symbolic links") ;
   puts(" -w  match exact string (whole-word search)") ;
   puts(" -/  Use backslash vs forward slash for path elements") ;
   puts("") ;
   puts(" -p  Search for name_component in the directories in the PATH variable") ;
   puts("     NOTE: This option replaces the normal ulocate functionality with") ;
   puts("     alternate functionality.  In this mode, subdirectories are NOT searched!!") ;
   puts("") ;
}

//************************************************************
bool loop_over_switches(char *p)
{
   while (*p != 0) {
      switch (*p) {
      //  output-format options
      case 'd':  output_format |= OFMT_DATE  ;  break;
      case 't':  output_format |= OFMT_TIME  ;  break;
      case 's':  output_format |= OFMT_SIZES ;  break;
      case 'S':  output_format |= OFMT_SIZEL ;  break;
      case 'n':  output_format |= OFMT_NAME  ;  break;

      case 'x':
         p++ ;
         size_len = (uint) atoi(p) ;
         if (size_len == 0)
             size_len = 6 ;
         break;
      
      //  other options
      case 'b':  debug = true ;  break;
      case 'v':  verbose = true ;  break;
      case 'l':  follow_symlinks = true ;  break;
      case 'p':  search_path = true ;  break;
      case 'w':  whole_word_search = true ;  break;
      case '/':  use_backslashes = true ;  break;

      default:
         usage() ;
         return false;
      }  //  switch 
      p++ ;
   }  //  while not done with current switch
   return true;
}

//************************************************************
int main (int argc, char **argv)
{
   int j;
   char *p;
   int strIdx = 0;
   temp_path[0] = 0;
   name_comp[0] = 0;

   puts (Version);

   //***********************************************************
   //  parse environment variable, *before* command line
   //***********************************************************
   int start_idx = 1 ;
   char *ulocate_env = getenv("ULOCATE");
   if (ulocate_env != NULL)
   {
      // argv[0] = ulocate_env ;
      // start_idx = 0 ;
      p = ulocate_env ;
      while (LOOP_FOREVER)
      {
         //  see if there are any further args in string
         char *next_arg = strchr(p, ' ');
         if (next_arg != NULL)
         {
            *next_arg++ = 0 ;
         }

         //  parse current string
         if (*p == '-')
         {
            p++ ;
            loop_over_switches(p);
         }
         else
         {
            //  non-switch arg in ULOCATE is *always* base search path
            strncpy (temp_path, p, sizeof (temp_path));
         }
         //  goto next arg, if any
         p = next_arg ;
         if (next_arg == NULL)
         {
            break;
         }
      }
   }

   // printf("size of MAX_PATH_LEN=%u bytes\n", MAX_PATH_LEN) ;  //  4096 bytes
   //***********************************************************
   //  parse command line
   //***********************************************************
   for (j = start_idx; j < argc; j++) {
      p = argv[j];
      if (*p == '-') {
         p++ ;
         if (*p == 0) {
            usage() ;
            return 1;
         }
         loop_over_switches(p);
      }  //  if switch is specified
      else {
         //  if ULOCATE environment variable is defined,
         //  treat any non-switch argument
         switch (strIdx) {
         case 0:
            strncpy (name_comp, p, sizeof (name_comp));
            break;

         case 1:
            strncpy (temp_path, p, sizeof (temp_path));
            break;

         default:
            usage ();
            return 1;
         }
         strIdx++;
      }
   }
   if (strlen (name_comp) == 0) {
      usage ();
      return 1;
   }

   // if (ulocate_env != NULL)
   // {
   //    strncpy (temp_path, ulocate_env, sizeof (temp_path));
   // }
   if (output_format == OFMT_NONE) {
       output_format = OFMT_NAME ;
   }
   printf("output format = 0x%04X, size_len=%u\n", output_format, size_len) ;

   //***********************************************************
   //  search the PATH (alternate mode)
   //***********************************************************
   if (search_path) {
      printf ("searching the PATH for %s\n", name_comp);
      search_path_for_name() ;
   }
   //***********************************************************
   //  normal subdirectory search
   //***********************************************************
   else {
      if (strlen(temp_path) == 0) {
         strcpy(temp_path, ".") ;
      }
      // qualify (target_path);
      p = realpath(temp_path, target_path) ;
      if (p == 0) {
         perror(temp_path) ;
         return 1;
      }
      if (strlen(target_path) == 3) {
         target_path[2] = '/' ;
      }

      printf("searching %s\n", target_path);
      //***********************************************************
      //  scan and build directory tree
      //***********************************************************
      build_dir_tree(target_path); //  read and build the dir tree
      test_display_state() ;
   }

   //***********************************************************
   //  traverse the discovered directory tree
   //***********************************************************
   // display_dir_tree (top);

   return 0;
}

