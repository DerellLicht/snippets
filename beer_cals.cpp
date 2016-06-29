//*********************************************************************************
//  Copyright (c) 2013-2015 Daniel D Miller
//  beer_cals.cpp
//  A utility to estimate calorie content of beer/ale, based on volume
//  and alcohol content.  
//  Calories from alcohol is based on standard 6.9 calories/gram.
//  Calories from carbs is harder to compute, because carb content is based
//  on factors (starting/ending specific gravity) that are not typically 
//  available on the bottle label.  I am using as my baseline, the carb
//  content of Newcastle Brown Ale, a typical mid-strengh ale.
//  
//  Written by:  Danial D Miller
//*********************************************************************************

#include <stdio.h>
#include <stdlib.h>

//*******************************************************************
void usage(void)
{
   puts("Usage: beer_alc -mML -oOZ -pPCT") ;
   puts("Either -m or -o must be specified for volume,") ;
   puts("and -p must be specified for percent ABV.") ;
}

//*******************************************************************
int main(int argc, char **argv)
{
   double ounces = 0.0 ;
   double pct_alc = 0.0 ;
   int idx ;
   for (idx=1; idx<argc; idx++) {
      char *p = argv[idx] ;
      if (*p == '-') {
         p++ ;
         switch (*p) {
         case 'm':
            p++ ;
            ounces = strtod(p, NULL) ;   //  this is actually ml
            ounces /= 29.574 ;   //  29.574 ml/oz
            break;

         case 'o':
            p++ ;
            ounces = strtod(p, NULL) ;
            break;

         case 'p':
            p++ ;
            pct_alc = strtod(p, NULL) ;
            break;
         }
      } else {
         usage() ;
         return 1;
      }
   }

   //  validate inputs
   if (ounces < 0.1  ||  pct_alc < 0.1) {
      usage() ;
      return 1;
   }

   printf("calculating calories for %.1f ounces of ale at %.1f ABV\n", ounces, pct_alc) ;

   //                                            g/oz   cal/g
   double cal_alc = ounces * (pct_alc / 100.0) * 28.3 * 6.9 ;
   //  carbs are harder to calculate, because they are not directly
   //  related to alcohol content.  Essentially, carb concentration is dependent
   //  on things such as density of the grain (i.e., how much unfermentable content
   //  there is in the grain) and initial vs final specific gravity.
   //  So this calculation is using some guesswork based on mid-strength ales.
   //  new values from 09/2015: http://www.shapefit.com/weight-loss/alcohol-calories.html
   // Beer, regular (12 fl oz)	13g  / 12oz
   // Beer, light (12 fl oz)	   4.5g / 12oz
   //  Per http://www.calorieking.com/foods/calories-in-ales-beers-brown-ale_f-ZmlkPTExNTM1Ng.html
   //  Newcastle Brown Ale (a mid-strength English ale) has 12.8g carbs in 12oz bottle,
   //  which roughly agrees with the previous calculation.
   double cal_carb = ounces * (13.0 / 12.0) ;
   printf("alcohol calories: %4u\n", (unsigned) cal_alc) ;
   printf("carbs calories:   %4u\n", (unsigned) cal_carb) ;
   printf("total calories:   %4u\n", (unsigned) (cal_alc + cal_carb)) ;
   return 0;
}

