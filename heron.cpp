//***********************************************************************
//  heron.cpp - This program implements Heron's Formula,
//  which calculates the area of an irregular triangle,
//  given only the lengths of the three sides, but no angles.
//  
//  Copyright 2023  Daniel D Miller
//***********************************************************************

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

//lint -esym(818, argv)   Pointer parameter could be declared as pointing to const
//lint -e10               Expecting '}'
//lint -esym(119, strtod) Too many arguments (2) for prototype 'strtod(const char *)'

//***********************************************************************
static void usage(void)
{
   puts("Usage: heron side1_len side2_len side3_len");
   puts("");
   puts("This program implements Heron's Formula,");
   puts("which calculates the area of an irregular triangle,");
   puts("given only the lengths of the three sides, but no angles.");
   puts("");
}

//***********************************************************************

int main(int argc, char** argv)
{
   double side1 = 0.0 ;
   double side2 = 0.0 ;
   double side3 = 0.0 ;
   
   usage();
   
   //  parse command line
   int idx ;
   int sides_count = 0 ;
   for (idx=1; idx<argc; idx++) {
      char *p = argv[idx];
      switch (sides_count) {
      case 0:
         side1 = strtod(p, NULL);
         sides_count++ ;
         break ;
         
      case 1:
         side2 = strtod(p, NULL);
         sides_count++ ;
         break ;
         
      case 2:
         side3 = strtod(p, NULL);
         sides_count++ ;
         break ;
         
      default:
         return 1 ;
      }
   }
   
   double min_valid_value = 0.001 ;
   if (sides_count != 3  ||
       side1 < min_valid_value  ||
       side2 < min_valid_value  ||
       side3 < min_valid_value) {
      return 1 ;      
   }
   
   printf("parameters: side1: %5.3f, side2: %5.3f, side3: %5.3f\n", side1, side2, side3);
   
   //  compute the summ parameter
   double sum = (side1 + side2 + side3) / 2.0 ;
   printf("intermediate sum parameter: %5.3f\n", sum);
   
   double total_area = sqrt(sum * (sum-side1) * (sum-side2) * (sum-side3)) ;
   printf("area: %5.3f\n", total_area);
   
   return 0;
}

