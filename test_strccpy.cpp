#include <windows.h>
#include <stdio.h>

#define  LOOP_FOREVER   true

//**********************************************************************************
//  copy chars from src to dest until comma or 0 are encountered
//**********************************************************************************
char *strccpy(char *src, char *dest, unsigned max_len)
{
   if (src == NULL  ||  dest == NULL) {
      return NULL ;
   }
   unsigned slen = 0 ;
   while (LOOP_FOREVER) {
      if (*src == ','  ||  *src == 0  ||  slen >= max_len) {
         *dest = 0 ;
         if (*src == ',') {
            src++ ;  //  skip terminating character
         }
         
         return (slen >= max_len) ? NULL : src ;
      }
      *dest++ = *src++ ;
   }
}

//**********************************************************************************
// Symbol/CUSIP,Description,Quantity,Price,Beginning Value,Ending Value,Cost Basis
char *test_str = (char *) "FNCMX,FIDELITY NASDAQ COMPOSITE INDEX ,78.74000,154.38000,10385.02,12155.88,7781.96" ;
//**********************************************************************************
#define  MAX_TEST_STR   260
int main(void)
{
   char *src = test_str ;
   char dest[MAX_TEST_STR+1];
   int lcount = 0 ;
   while (LOOP_FOREVER) {
      src = strccpy(src, dest, MAX_TEST_STR);
      if (src == NULL) {
         printf("unexpected termination\n");
         break ;
      }
      printf("%u: [%s]\n", lcount, dest);
      if (*src == 0) {
         break ;
      }
      lcount++ ;
   }
   return 0 ;
}

