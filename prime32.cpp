//*****************************************************************
//  PRIME32.CPP:                                                   
//  This program reads in a number, determines whether it          
//  is a prime, and prints either the number or its factors.       
//                                                                 
//  written by:   Daniel D. Miller                                 
//                                                                 
//*****************************************************************
//  Maximum integer input: 18,446,744,073,709,551,615              
//                                                                 
//  Largest found prime:    2,041,945,040,465,531,213              
//*****************************************************************
//  Version    Date        Description
//   1.00      07/23/05    Original
//   1.01      02/22/12    Fix bug on N*N, with N prime
//*****************************************************************

#include <stdio.h>
#include <stdlib.h>

char tempstr[81] ;

enum {
D_STANDARD = 0,
D_INTERACTIVE
} ;

//**********************************************************************
int main(int argc, char** argv)
{
   unsigned __int64 givennbr, nbrleft, nextodd;
   int power, prime ;
   int next = 1 ;
   int display = (argc > 1) ? D_STANDARD : D_INTERACTIVE ;

   puts("PRIME32.EXE - Written by: Daniel D. Miller") ;
   puts("****************************************************");
   puts("This program determines whether a number is a prime,");
   puts("then displays either the number or its factors.");

   do {  /* Repeat main program until entry = 0 */
      prime = 1 ;  givennbr = power = 0L ;
      switch (display)
         {
         case D_INTERACTIVE:
            puts("");
            printf("Enter a number (0 quits): ");
            gets(tempstr) ;
            givennbr = _atoi64(tempstr) ;
            if (givennbr == 0)
               return 0;
            break;
         case D_STANDARD:
            if (argv[next] == NULL)
               exit(0) ;
            puts("****************************************************\n");
            givennbr = _atoi64(argv[next++]) ;
            if (givennbr == 0)
               return 0;
            break;
         }

      //****************************************************
      //  factor out powers of 2
      //****************************************************
      nbrleft = givennbr;
      if (!(nbrleft & 1)) {
         while (!(nbrleft & 1)) {      /* while nbrleft mod 2 = 0  */
            power++ ;
            nbrleft /= 2 ;
         }
         if (prime) {
            printf("The factors of %I64u are:\n\n", givennbr) ;
            printf("          Factor                       Power \n");
            prime = 0 ;
         }
         printf("   %20I64u       %20I64u\n", (__int64) 2, (__int64) power) ;
      }

      //****************************************************
      //  factor out odd powers
      //****************************************************
      nextodd = 3;
      while ((nbrleft / nextodd) >= nextodd) {
         power = 0 ;
         if (nbrleft % nextodd == 0) {
            while (nbrleft % nextodd == 0) {
                nbrleft /= nextodd ;
                power++ ; 
            }
            if (prime) {
               printf("The factors of %I64u are:\n\n", givennbr) ;
               printf("          Factor                       Power \n");
               prime = 0 ;
            }
            printf("   %20I64u       %20I64u\n", nextodd, (__int64) power) ;
         }  /* IF nbrleft%nextodd    */
         nextodd += 2;
      }     /* while nextodd*nbrleft <= nbrleft */

      //****************************************************
      //  check for remaining value
      //****************************************************
      if (nbrleft != 1) {
         if (prime)
            printf("%I64u is a prime number\n", givennbr) ; 
         else
            printf("   %20I64u       %20I64u\n", nbrleft, (__int64) 1) ;
      }
   }
   while (givennbr != 0);

   return 0 ;
}

