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
//  Largest found prime:   12,764,787,846,358,441,471
//                               12764787846358441471
//*****************************************************************
//  Version    Date        Description
//   1.00      07/23/05    Original
//   1.01      02/22/12    Fix bug on N*N, with N prime
//   1.02      01/11/16    Change number-read function to strtoull()
//*****************************************************************

#include <stdio.h>
#include <stdlib.h>

//lint -esym(818, argv)  could be declared as pointing to const
//lint -esym(534, gets)  Ignoring return value of function

char tempstr[81] ;

enum {
D_STANDARD = 0,
D_INTERACTIVE
} ;

//**********************************************************************
int main(int argc, char** argv)
{
   unsigned __int64 givennbr, nbrleft, nextodd;
   int power ;
   int next = 1 ;
   int display = (argc > 1) ? D_STANDARD : D_INTERACTIVE ;

   puts("PRIME32.EXE - Written by: Daniel D. Miller") ;
   puts("****************************************************");
   puts("This program determines whether a number is a prime,");
   puts("then displays either the number or its factors.");

   do {  /* Repeat main program until entry = 0 */
      givennbr = power = 0L ;
      switch (display) {
      case D_INTERACTIVE:
         puts("");
         printf("Enter a number (0 quits): ");
         gets(tempstr) ;   //lint !e421  dangerous function
         givennbr = strtoull(tempstr, NULL, 10) ;
         break;
         
      // case D_STANDARD:
      default:
         if (argv[next] == NULL)
            exit(0) ;
         puts("****************************************************\n");
         givennbr = strtoull(argv[next++], NULL, 10) ;
         break;
      }
      if (givennbr == 0) {
         break ;
      }
      
      //****************************************************
      //  factor out powers of 2
      //****************************************************
      bool header_displayed = false ;
      nbrleft = givennbr;
      if ((nbrleft & 1) == 0) {
         while ((nbrleft & 1) == 0) {      /* while nbrleft mod 2 = 0  */
            power++ ;
            nbrleft >>= 1 ;
         }
         printf("The factors of %I64u are:\n\n", givennbr) ;   //lint !e539 Did not expect positive indentation
         printf("          Factor                       Power \n");
         header_displayed = true ;
         //  Okay, I get it... when the preceding while loop is done,
         //  if original number was multiple of 2,
         //  then nbrleft will equal 1, that being the power sign
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
            if (!header_displayed) {
               printf("The factors of %I64u are:\n\n", givennbr) ;
               printf("          Factor                       Power \n");
               header_displayed = true ;
            }
            printf("   %20I64u       %20I64u\n", nextodd, (__int64) power) ;
         }  /* IF nbrleft%nextodd    */
         nextodd += 2;
      }     /* while nextodd*nbrleft <= nbrleft */
      // printf("DEBUG: %I64u %I64u\n", nbrleft, nextodd) ;
      // DEBUG: 9851141 3139
      // 9851141 is a prime number

      //****************************************************
      //  check for remaining value
      //****************************************************
      if (!header_displayed) {
         printf("%I64u is a prime number\n", givennbr) ; 
      }
      else if (nbrleft > 1) {
         printf("   %20I64u       %20I64u\n", nbrleft, (__int64) 1) ;
      }
   }
   while (givennbr != 0);

   return 0 ;
}

