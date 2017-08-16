#include <stdio.h>

char ctrl_codes[32][4] = {
   "NUL", "SOH", "STX", "ETX", "EOT", "ENQ", "ACK", "BEL", 
   "BS ", "HT ", "LF ", "VT ", "FF ", "CR ", "SO ", "SI ", 
   "DLE", "DC1", "DC2", "DC3", "DC4", "NAK", "SYN", "ETB",
   "CAN", "EM ", "SUB", "ESC", "FS ", "GS ", "RS ", "US " } ;

int main(void)
{
   unsigned avalue = 0 ;
   unsigned col, row ;

   puts("ch dec hex ctl|ch dec hex|ch dec hex|ch dec hex|ch dec hex|ch dec hex|ch dec hex|ch dec hex") ;
   puts("==============|==========|==========|==========|==========|==========|==========|==========") ;
   for (row=0; row<32; row++) {
      avalue = row ;
      for (col=0; col<8; col++) {
         // avalue = row + (col * rows) ;
         if (avalue < 32) {
            printf("^%c %03d x%02X %s", '@'+avalue, avalue, avalue, ctrl_codes[avalue]) ;
         } else {
            printf(" %c %03d x%02X", avalue, avalue, avalue) ;
         }
         if (col < 7) 
            printf("|") ;
         avalue += 32 ;
      }
      puts("") ;
   }
   return 0;
}

