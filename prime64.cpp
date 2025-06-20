//*****************************************************************
//  PRIME64.CPP:                                                   
//  This program reads in a number, determines whether it          
//  is a prime, and prints either the number or its factors.       
//                                                                 
//  written by:   Daniel D. Miller                                 
//                                                                 
//*****************************************************************
//  Maximum integer input (64-bit): 18,446,744,073,709,551,615
//                                  18446744073709551615      
//                                                   
//  Largest found prime:            18,446,744,073,709,551,557
//                                  18446744073709551557      
//
//  Other primes:
//                                  6148914691236517199
//*****************************************************************
//  Compare 32-bit and 64-bit builds:
// 
// 18446744073709551557 is a prime number (32 bit)
// Timer 1 off: 14:11:41  Elapsed: 0:00:43.11
// 
// 18446744073709551557 is a prime number (64 bit)
// Timer 1 off: 14:08:40  Elapsed: 0:00:30.28  (old method)
// Timer 1 off: 11:04:24  Elapsed: 0:00:15.14  (new method)
// 
// 64-bit version on new Ryzen 7 5800X3D CPU
// 18446744073709551557 is a prime number
// Timer 1 off: 18:15:23  Elapsed: 0:00:03.41
// 
//*****************************************************************
//  Version    Date        Description 
//   1.00      07/23/05    Original
//   1.01      02/22/12    Fix bug on N*N, with N prime
//   1.02      01/11/16    Change number-read function to strtoull()
//   1.03      10/10/22    In odd-number scan, replace second divide 
//                         (for MOD operation) with a multiply
//*****************************************************************

#include <stdio.h>
#include <stdlib.h>

#define  USE_LLU
// #undef  USE_LLU

static char const version_string[] = "1.03" ;

//lint -esym(818, argv)  could be declared as pointing to const
//lint -esym(534, gets)  Ignoring return value of function

#define  LOOP_FOREVER   true

#define  INP_STR_LEN    80
char tempstr[INP_STR_LEN+1] ;

enum {
D_STANDARD = 0,
D_INTERACTIVE
} ;


//**********************************************************************
//  this reports whether executable is 32-bit or 64-bit build
//**********************************************************************
unsigned get_build_size(void)
{
   return (sizeof(int*) == 8) ? 64 : 32 ;
}

//**********************************************************************
int main(int argc, char** argv)
{
   unsigned __int64 givennbr, nbrleft, nextodd;
   int power ;
   int next = 1 ;
   int display = (argc > 1) ? D_STANDARD : D_INTERACTIVE ;

   printf("PRIME%u V%s - Written by: Derell Licht\n", get_build_size(), version_string) ;
   puts("****************************************************");
   puts("This program determines whether a number is a prime,");
   puts("then displays either the number or its factors.");
   
   do {  /* Repeat main program until entry = 0 */
      givennbr = power = 0L ;
      switch (display) {
      case D_INTERACTIVE:
         puts("");
         printf("Enter a number (0 quits): ");
         // gets(tempstr) ;   //lint !e421  dangerous function  
         fgets(tempstr, INP_STR_LEN, stdin);
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
#ifdef  USE_LLU         
         printf("The factors of %llu are:\n\n", givennbr) ;
#else         
         printf("The factors of %I64u are:\n\n", givennbr) ;
#endif         
         printf("          Factor                       Power \n");
         header_displayed = true ;
         //  Okay, I get it... when the preceding while loop is done,
         //  if original number was multiple of 2,
         //  then nbrleft will equal 1, that being the power sign
#ifdef  USE_LLU         
         printf("   %20llu       %20llu\n", (__int64) 2, (__int64) power) ;
#else         
         printf("   %20I64u       %20I64u\n", (__int64) 2, (__int64) power) ;
#endif         
      }

      //****************************************************
      //  factor out odd powers
      //****************************************************
      nextodd = 3;
      //  this is an indirect way of saying 
      //  "stop at square root of current target [nextodd]"
      //  
      //  NOTE: NEW_METHOD replaces second divide (for MOD operation) with a multiply
      // while ((nbrleft / nextodd) >= nextodd) {  
      while (LOOP_FOREVER) {  
         unsigned __int64 nbrdiv = nbrleft / nextodd ;
         if (nbrdiv < nextodd) {
            break ;
         }
         power = 0 ;
         //  use multiply with previous calc, to avoid second division
         // if (nbrleft % nextodd == 0) {
         if ((nbrdiv * nextodd) == nbrleft) {
            //  Later note: this inner loop only executes repeatedly,
            //  if a large factor is present in a *very* large number, which is quite rare.
            //  otherwise, we don't get much penefit from the inner conversion loop,
            //  because we end up back out at the outer loop.
            // 
            // The factors of 18446744073709551597 are:
            //           Factor                       Power
            //                       3                          1
            //     6148914691236517199                          1
            // Timer 1 off: 12:18:23  Elapsed: 0:00:08.72
            // 
            do {
               nbrleft /= nextodd ;
               nbrdiv = nbrleft / nextodd ;
               power++ ;
            }
            // while (nbrleft % nextodd == 0) ;
            while ((nbrdiv * nextodd) == nbrleft) ;
            
            if (!header_displayed) {
#ifdef  USE_LLU         
               printf("The factors of %llu are:\n\n", givennbr) ;
#else               
               printf("The factors of %I64u are:\n\n", givennbr) ;
#endif               
               printf("          Factor                       Power \n");
               header_displayed = true ;
            }
#ifdef  USE_LLU         
            printf("   %20llu       %20llu\n", nextodd, (__int64) power) ;
#else            
            printf("   %20I64u       %20I64u\n", nextodd, (__int64) power) ;
#endif            
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
#ifdef  USE_LLU         
         printf("%llu is a prime number\n", givennbr) ; 
#else         
         printf("%I64u is a prime number\n", givennbr) ; 
#endif         
      }
      else if (nbrleft > 1) {
#ifdef  USE_LLU         
         printf("   %20llu       %20llu\n", nbrleft, (__int64) 1) ;
#else         
         printf("   %20I64u       %20I64u\n", nbrleft, (__int64) 1) ;
#endif         
      }
   }
   while (givennbr != 0);

   return 0 ;
}

