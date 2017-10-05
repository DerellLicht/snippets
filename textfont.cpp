//*******************************************************************
//  Copyright (c) 1994  Daniel D Miller
//  This software is released as freeware, with no restrictions on use,
//  commercial or otherwise.
//  
//  textfont.c - Displays font set on screen.
//  
//  build:  gcc -Wall -O2 textfont.c -o textfont.exe
//*******************************************************************

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <io.h>
#include <fcntl.h>

typedef unsigned char  u8;
typedef unsigned int   uint;

//lint -esym(534, putchar, _close)
//lint -esym(818, argv)

//***************************************************************
u8 user_font[256][20];
unsigned user_points ;

//*******************************************************************
//  This is a text-mode routine
//*******************************************************************
void show_binary(u8 bchar)
{
   u8 mask = 0x80 ;

   while (mask != 0)
   {
      if ((bchar & mask) == 0)
         putchar('.') ;
      else 
         putchar('x') ;
      mask >>= 1 ;
   }

   putchar('\n') ;
}

//******************************************************************
int read_font_file(char *fname)
{
   // return read_dos_font(fname) ;
   u8 font_bfr[256 * 20];
   unsigned fchar, frow ;

   memset(user_font, 0, sizeof(user_font)) ;

   int fhdl = _open(fname, O_RDONLY | O_BINARY) ;
   if (fhdl < 0) {
      printf("%s open: %s\n", fname, strerror(errno)) ;
      return -1 ;
   }
   int rbytes = _read(fhdl, font_bfr, sizeof(font_bfr)) ;
   if (rbytes < 0) {
      printf("%s read: %s\n", fname, strerror(errno)) ;
      return -1 ;
   }

   //  verify file length
   if (((uint) rbytes % 256) != 0) {
      printf("%s read: bad size (%d), not MOD256\n", fname, rbytes) ;
      return -1 ;
   }
   user_points = (uint) rbytes / 256;
   _close(fhdl) ;

   //  read font into our user font table
   u8 *fptr = &font_bfr[0];
   for (fchar = 0; fchar < 256; fchar++) {
      for (frow = 0; frow < user_points; frow++) {
         user_font[fchar][frow] = *fptr++;
      }
   }
   // wsprintf(errstr, "%s: %u points\n", fname, user_points) ;
   // OutputDebugString(errstr) ;
   return 0;
}

//***************************************************************
int main(int argc, char **argv)
{
#define  MAX_FNAME_LEN  1024    
    char fname[MAX_FNAME_LEN+1] = "" ;
    int idx ;
    for (idx=1; idx<argc; idx++)
    {
        char *p = argv[idx];
        strncpy(fname, p, MAX_FNAME_LEN);
        fname[MAX_FNAME_LEN] = 0 ;  // ensure NULL-term
    }

    if (fname[0] == 0)
    {
        puts("Usage: textfont font_filename");
        return 1;
    }
    int result = read_font_file(fname);
    if (result < 0)
    {
        return 1;
    }
    
    //*************************************************
    //  font_map[] should be valid - use it.
    //*************************************************
    //      show_binary(font_map[curchar][k]) ;
    //*************************************************
    uint curchar = 97 ;   //  start with lowercase 'a'
    uint uidx ;
    int done = 0 ;
    while (done == 0)
       {
       printf("\nThis is char #%u: \n", curchar) ;
       for (uidx=0; uidx<user_points; uidx++)
          {
          show_binary(user_font[curchar][uidx]) ;
          }

       puts("\npress ESC to exit, any other key to show it") ;
       int inpint = getch() ;
       u8 inchr = (u8) inpint ;
       switch (inchr)
          {
          case 27:
             done = 1 ;
             break;

          default:
             curchar = (uint) inchr ;
             break;
          }
       }
    return 0 ;
}


