//**********************************************************************************
//  readall.cpp 
//  This is a single-file solution for reading all the files in a specified location.
//  
//  This file does not do any of the special functions that my normal,
//  dere_libs-based projects do, such as:
//  - expanding the specified file spec to a fully-qualified form
//  - handling UNICODE
//  - providing access to debug/info functions such as syslog() and get_system_message()
//  If such expanded capabilities are required, I recommend copy my uni_file_mgr
//  repo and working from that.  But if you just want a single-file option that
//  just reads the files and saves them in a vector, this is your solution.
//  
//  Written by:  Derell Licht
//  build: g++ -Wall -O2 read_files.cpp -o read_files.exe
// 
//  lint:  clang-tidy read_files.cpp -- 
//**********************************************************************************

#include <windows.h>
#include <cstdio>
#include <string>
#include <vector>

using uchar = unsigned char ;
using uint  = unsigned int ;
using ulong = unsigned long ;

//  this definition was excluded by WINNT.H
#define FILE_ATTRIBUTE_VOLID  0x00000008

//  per Jason Hood, this turns off MinGW's command-line expansion, 
//  so we can handle wildcards like we want to.                    
int _CRT_glob = 0 ;

static uint filecount = 0 ;

//lint -esym(843, show_all)
static bool show_all = true ;

//**********************************************************************************
//  constructor for ffdata struct
//**********************************************************************************
struct ffdata 
{
    DWORD attrib {};
    FILETIME ft {};
    ULONGLONG fsize {};
    std::string filename {};
    std::string name {};
    std::string ext {};
    bool dirflag {} ;
    ffdata(DWORD sattrib, FILETIME sft, ULONGLONG sfsize, std::string sfilename, bool sdirflag );
} ;

ffdata::ffdata(
    DWORD sattrib,
    FILETIME sft,
    ULONGLONG sfsize,
    std::string sfilename,
    bool sdirflag
) :
attrib(sattrib),
ft(sft),
fsize(sfsize),
filename(std::move(sfilename)),
dirflag(sdirflag)
{}

static std::vector<ffdata> flist;

//**********************************************************************************
// int read_files(std::string filespec)
auto read_files(const std::string &filespec) -> int
{
   WIN32_FIND_DATA fdata ; //  long-filename file struct
   HANDLE handle = FindFirstFile (filespec.c_str(), &fdata);
   //  according to MSDN, Jan 1999, the following is equivalent
   //  to the preceding... unfortunately, under Win98SE, it's not...
   // handle = FindFirstFileEx(target[i], FindExInfoStandard, &fdata, 
   //                      FindExSearchNameMatch, NULL, 0) ;
   if (handle == INVALID_HANDLE_VALUE) {
      return -errno;
   }

   //  loop on find_next
   bool fn_okay = false;
   bool done = false;
   while (!done) {
      if (!show_all) {
         if ((fdata.dwFileAttributes & 
            (FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_READONLY | FILE_ATTRIBUTE_SYSTEM)) != 0) {
            goto search_next_file;  //  NOLINT
         }
      }
      //  filter out directories if not requested
      if ((fdata.dwFileAttributes & FILE_ATTRIBUTE_VOLID) != 0)
         fn_okay = false;
      else if ((fdata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != FILE_ATTRIBUTE_DIRECTORY)
         fn_okay = true;
      //  For directories, filter out "." and ".."
      else if (strcmp(fdata.cFileName, ".") == 0)
         fn_okay = false;
      else if (strcmp(fdata.cFileName, "..") == 0)
         fn_okay = false;
      else
         fn_okay = true;

      if (fn_okay) {
         filecount++;

         //  allocate and initialize the structure
         flist.emplace_back( fdata.dwFileAttributes,
                             fdata.ftCreationTime,
                            (fdata.nFileSizeHigh * 1LL<<32) + fdata.nFileSizeLow, //  NOLINT
                             fdata.cFileName,
                            (fdata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) ? true : false);
      }  //  if file is parseable...

search_next_file:
      //  search for another file
      if (FindNextFile (handle, &fdata) == 0) {
         done = true;
      }
   }  //  end while

   FindClose (handle);
   return 0;
}

//************************************************************************
static uint max_filename_len = 0 ;

void calc_max_filename_len(ffdata& ftemp)
{
   uint flen = ftemp.filename.length();
   if (max_filename_len < flen) {
       max_filename_len = flen ;
   }
}  

//**********************************************************************************
int main(int argc, char **argv)  // NOLINT
{
   std::string file_spec {};
   
   for (int idx=1; idx<argc; idx++) {
      char *p = argv[idx] ;   // NOLINT
      file_spec = p ;
   }

   if (file_spec.length() == 0) {
      puts("Usage: readall <filespec>");
      return -1;
   }

   int result = read_files(file_spec);
   if (result < 0) {
      printf("Error: filespec: %s, %s\n", file_spec.c_str(), strerror(-result));
   }
   else {
      printf("result: %d, file count: %u\n", result, filecount);
      if (filecount > 0) {
      
         //  find max filename length
         //  This is used in formatting console output messages in tables
         for(auto &file : flist) {
            calc_max_filename_len(file);
         }
         
         // for(auto &file : flist) {
            // print_file_info(file);
            // printf("%s\n", file.filename.c_str());
         // }
         
         printf("%u files, Max filename length: %u chars\n", filecount, max_filename_len);
      }
   }
   return 0;
}

