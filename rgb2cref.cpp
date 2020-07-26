#include <windows.h>
#include <stdio.h>
#ifdef _lint
#include <stdlib.h>  //  strtoul()
#endif
//lint -esym(818, argv)

typedef unsigned int   uint ;
typedef unsigned char  u8 ;

typedef union ul2uc_u {
   uint ul ;
   u8   uc[4] ;
} ul2uc_t ;

//********************************************************************************
void usage(void)
{
   puts("Usage: rgb2cref [ 0xRRGGBB or rrr.ggg.bbb ]") ;
   puts("rgb2cref 0xRRGGBB outputs equivalent color in rrr.ggg.bbb") ;
   puts("rgb2cref rrr.ggg.bbb outputs equivalent color in 0xRRGGBB") ;
}

//********************************************************************************
int main(int argc, char **argv)
{
   ul2uc_t uconv ;
   if (argc == 1) {
      usage() ;
      return 1;
   }
   bool to_rgb = false ;
   char *instr = argv[1] ;
   if (strncasecmp(instr, "0x", 2) == 0) {
      to_rgb = true ;
   }

   //  now convert and display the result
   if (to_rgb) {
      uconv.ul = (uint) strtoul(instr, NULL, 16) ;
   } else {
      uint utemp ;
      char *cptr ;
      uconv.ul = 0 ;
      utemp = (uint) strtoul(instr, &cptr, 10) ;
      if (cptr == NULL) {
         usage() ;
         return 1;
      }
      cptr++ ; //  skip the dot
      uconv.uc[2] = (u8) utemp ;
      utemp = (uint) strtoul(cptr, &cptr, 10) ;
      if (cptr == NULL) {
         usage() ;
         return 1;
      }
      cptr++ ; //  skip the dot
      uconv.uc[1] = (u8) utemp ;
      utemp = (uint) strtoul(cptr, NULL, 10) ;
      uconv.uc[0] = (u8) utemp ;
   }
   printf("0x%06X (%10u)= %u.%u.%u\n", uconv.ul, uconv.ul, 
      (uint) uconv.uc[2],
      (uint) uconv.uc[1],
      (uint) uconv.uc[0]) ;
   
   return 0;
}

