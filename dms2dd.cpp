//********************************************************************************
//  convert between degrees/minutes/seconds and decimal degrees
//  http://maps2.nris.state.mt.us/topofinder1/LatLong.asp
//********************************************************************************

#include <stdio.h>
#include <stdlib.h>  //  strtod
#include <math.h>

typedef unsigned int  uint ;

typedef struct dms_s {
   int   degs ;
   uint  mins ;
   uint secs ;
} dms_t, *dms_p ;

//********************************************************************************
static void usage(void)
{
   puts("Usage: dms2dd input_value") ;
   puts("Enter dms as degrees.minutes.seconds") ;
   puts("Enter dd as integer_degrees.fractional_degrees") ;
   puts("If dms2dd sees two decimal points in the input, ") ;
   puts("the value will be treated as dms, and dd will be calculated and output.") ;
   puts("If dms2dd sees one decimal points in the input, ") ;
   puts("the value will be treated as dd, and dms will be calculated and output.") ;
}

//********************************************************************************
static uint count_dec_points(char *inpstr)
{
   int decpts = 0 ;
   while (*inpstr != 0) {
      if (*inpstr == '.')
         decpts++ ;
      inpstr++ ;
   }
   return decpts;
}

//********************************************************************************
// Conversion from Decimal Degrees to Degrees, Minutes, and Seconds
//********************************************************************************
dms_p convert_dd_to_dms(char *inpstr)
{
   static dms_t dms_data ;
   double ddegs, dmins, frac, dtemp ;
   double dd_value = strtod(inpstr, NULL) ;
   bool is_negative = false ;
   if (dd_value < 0) {
      is_negative = true ;
      dd_value *= -1.0 ;
   }
   
   // Decimal Degree =  D.ddd    46.17444
   // Degrees (D) =  D.ddd - .ddd   46.17444 - 0.17444 = 46
   frac = modf(dd_value, &ddegs) ;

   // Decimal Minutes (M.mmm) =  .ddd * 60   0.17444 * 60 = 10.4664
   dtemp = frac * 60.0 ;

   // Minutes (M) =  M.mmm - .mmm   10.4664 - 0.4664 = 10
   frac = modf(dtemp, &dmins) ;

   // Seconds (S) =  .mmm * 60   0.4664 * 60 = 27.984
   dtemp = frac * 60.0 ;
   uint secs = (uint) dtemp ;

   // D/M/S =  D and M and S  46 Degrees, 10 Minutes, 27.984 Seconds
   uint mins = (uint) dmins ;
   int degs = (int) ddegs ;
   dms_data.degs = (is_negative) ? -degs : degs ;
   dms_data.mins = mins ;
   dms_data.secs = secs ;
   return &dms_data ;
}

//********************************************************************************
// Conversion from Degrees, Minutes, and Seconds to Decimal Degrees
// 
// Example: 46 degrees, 10 minutes, 28 seconds
// 
// Decimal Degrees =    Degrees + (Minutes/60) + (Seconds/3600)
// Decimal Degrees =    46 + (10/60) + (28/3600)
// Decimal Degrees =    46 + (0.1666667) + (0.0077778)
// Decimal Degrees =    46.17444 
//********************************************************************************
double convert_dms_to_dd(char *inpstr)
{
   int degs, mins, secs ;
   char *next_part ;
   degs = (int) strtol(inpstr, &next_part, 10) ;
   next_part++ ;
   mins = (int) strtol(next_part, &next_part, 10) ;
   next_part++ ;
   secs = (int) strtol(next_part, NULL, 10) ;

   bool is_negative = (degs < 0) ? true : false ;
   if (is_negative) {
      degs = - degs ;
   }

   double ddegs =  (double)  degs ;
   double dmins = ((double) (mins) / 60.0) ;
   double dsecs = ((double) (secs) / 3600.0) ;
   // printf("parts: ddegs=%.4f, dmins=%.4f, dsecs=%.4f\n", ddegs, dmins, dsecs) ;
   // double dd_value = ddegs + dmins + dsecs ;
   double dd_value = ddegs + dmins + dsecs ;
   if (is_negative)
      dd_value *= -1.0 ;
   return dd_value;
}

//********************************************************************************
int main(int argc, char **argv)
{
   char *inpstr = 0 ;
   int decpts = 0 ;
   dms_p dmsptr ;
   double dd_value ;

   //  parse command line
   int j ;
   for (j=1; j<argc; j++) {
      char *p = argv[j] ;
      decpts = count_dec_points(p) ;
      inpstr = p ;
   }

   //  validate inputs
   if (inpstr == 0) {
      usage() ;
      return 1;
   }

   switch (decpts) {
   case 1:
      dmsptr = convert_dd_to_dms(inpstr) ;
      printf("%s = %d.%u.%u\n", inpstr, dmsptr->degs, dmsptr->mins, dmsptr->secs) ;
      break;

   case 2:
      dd_value = convert_dms_to_dd(inpstr) ;
      printf("%s = %.4f\n", inpstr, dd_value) ;
      break;

   default:
      usage() ;
      return 1;
   }
   return 0;
}

