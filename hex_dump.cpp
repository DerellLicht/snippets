//***************************************************************************
//  HEX_DUMP.CPP: Read data and display as both binary and ASCII data.
//  
//  Written by:   Daniel D. Miller
//  
//  Last Update:  08/15/17 19:56
//***************************************************************************
#include <stdio.h>
#include <string.h>
#include <io.h>
#include <fcntl.h>
#include <sys\stat.h>

#define  LOOP_FOREVER   1

//lint -e737  Loss of sign in promotion from int to unsigned int
//lint -esym(534, _close)   //  ignoring return value
//lint -esym(818, argv)  // could be declared as pointing to const

typedef  unsigned char  uint8 ;
typedef  unsigned int   uint ;

static uint8 inframe[260] ;
static char infile[260] ;

//***************************************************************************
static uint hex_dump(uint8 const * const bfr, uint bytes, uint base_index)
{
   uint j, len ;
   char tail[40] ;
   char pstr[81] ;
   static bool hex_dump_active = false ;
   if (hex_dump_active)
      return 0;
   hex_dump_active = true ;

   tail[0] = 0 ;
   uint idx = 0 ;
   uint plen = 0 ;
   while (LOOP_FOREVER) {
      uint leftovers = bytes - idx ;
      if (leftovers > 16)
          leftovers = 16 ;

      plen = sprintf(pstr, "%05X:  ", base_index+idx) ;  //lint !e737 !e732
      len = 0 ;
      for (j=0; j<leftovers; j++) {
         uint8 chr = bfr[idx+j] ;
         plen += sprintf(&pstr[plen], "%02X ", chr) ;
         if (chr < 32) {
            len += sprintf(tail+len, ".") ;
         } else if (chr < 127) {   //lint !e506 !e774
            len += sprintf(tail+len, "%c", chr) ;
         } else {
            len += sprintf(tail+len, "?") ;
         }
      }
      //  last, print fill spaces
      for (; j<16; j++) {
         plen += sprintf(&pstr[plen], "   ") ;
         len += sprintf(tail+len, " ") ;
      }

      // printf(" | %s |\n", tail) ;
      strcat(pstr, " | ") ;
      strcat(pstr, tail) ;
      strcat(pstr, " |") ;
      printf("%s\n", pstr) ;

      idx += leftovers ;
      if (idx >= bytes)
         break;
   }
   base_index += idx ;
   hex_dump_active = false ;
   return base_index;
}


//*************************************************************
int main(int argc, char** argv)
{
   int inhdl, inbytes ;
   uint base_index = 0 ;
   
   if (argc != 2) {
      puts("Usage: hex_dump filename") ;
      return 1;
   }
   strcpy(infile, argv[1]) ;

   inhdl = _open(infile, _O_RDONLY | _O_BINARY, _S_IREAD) ;
   if (inhdl == -1) {
      perror(infile) ;
      return 1;
   }
   while (1) {
      //  this should read multiples of 16 bytes until EOF,
      //  so lines display evenly...
      inbytes = _read(inhdl, inframe, 256) ;
      if (inbytes == 0)
         break;   //  we're done
      if (inbytes < 0) {
         perror(infile) ;
         return 1;
      }
      base_index = hex_dump(&inframe[0], (uint) inbytes, base_index) ;
   }

   _close(inhdl) ;
   return 0;
}         

