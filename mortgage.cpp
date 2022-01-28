//*****************************************************************
//*****************************************************************
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

//lint -esym(119, strtod) //Too many arguments (2) for prototype 'strtod(const char *)'
//lint -esym(818, argv)   // Pointer parameter 'argv' (line 147) could be declared as pointing to const

typedef unsigned int  uint ;

static double balance = 0.0 ;
static double i = 0.0 ;
static double N = 0.0 ;
static double overpay = 0.0 ;
static double r = 0.0 ;

//*****************************************************************
void usage(void)
{
   puts("Usage: MORTGAGE principle interest number_months overpay");
   puts("In normal operation, this program computes the monthly payment,") ;
   puts("and payment summary table, for a loan with the input parameters.") ;
   puts("") ;
   puts("Alternate mode:") ;
   puts("If number_months is 0, overpay is treated as the payment amount,") ;
   puts("and this program will calculate and display the number of months") ;
   puts("required to pay off principle.") ;
   puts("") ;
   puts("Also, if overpay is greater than payment, then overpay is treated as") ;
   puts("total payment amount, and overpay is calculated from that.") ;
   puts("") ;
}

//*****************************************************************
static void compute_mortgage_payment(void)
{
   double payment, principle, interest;
   double yprinciple, yinterest ;
   double tprinciple, tinterest;
   unsigned j, k ;
   int ibalance;

   //  calculate standard payment
   payment = (balance * r) / (1.0 - (1.0 / pow ((1.0 + r), N)));

   if (overpay > payment) {
      overpay = overpay - payment ;
   }

   //  display input parameters
   puts ("input values:");
   printf ("Starting Principle: $%.0f, loan length: %.0f months\n", balance, N);
   printf ("Interest rate: %.3f, overpay: $%.2f per month\n\n", i, overpay);

   printf ("basic monthly payment = %.2f\n\n", payment);

   //  display amortization table
   puts ("year    interest  principle   balance");
   puts ("====    ========  =========  =========");

   tprinciple = tinterest = 0.0;
   j = 1;
   ibalance = (int) (balance * 100) ;
   // while (balance > 0.00) {
   while (ibalance > 0) {
      yprinciple = 0;
      yinterest = 0;
      for (k = 0; k < 12; k++) {
         interest = balance * r;
         if (balance + interest > payment) {
          principle = payment - interest + overpay;
            balance -= principle;
            yinterest += interest;
            yprinciple += principle;
         }
         else {
            yinterest += interest;
            yprinciple += balance;
            balance = 0.0;
            break;
         }
      }
      tprinciple += yprinciple;
      tinterest += yinterest;

      printf ("%02u:    %9.2f  %9.2f  %9.2f\n", j, yinterest, yprinciple, balance);
      ibalance = (int) (balance * 100) ;
      j++;
   }

   //  display totals paid
   puts ("====   =========  =========  =========");
   printf ("total: %9.2f  %9.2f  %9.2f\n",
      tinterest, tprinciple, tprinciple + tinterest);
}

//*****************************************************************
static void compute_months_remaining(void)
{
   double remaining_balance = balance ;
   double payment = overpay ;
   double tprinciple = 0.0, tinterest = 0.0;
   double yprinciple = 0.0, yinterest = 0.0;
   double principle ;

   double interest = balance * r;
   if (payment < interest) {
      printf("Cannot pay off, interest (%.2f) is greater than payment (%.2f)\n", interest, payment) ;
      return ;
   }
   printf("initial balance = %.2f, payment is %.2f\n", balance, payment) ;
   puts("") ;
   puts("year   principle   interest   balance") ;
   puts("=====  =========  =========  =========");

   uint month = 0 ;
   uint year = 0 ;
   bool done = false ;
   while (remaining_balance > 0.01) {      //  848.65
      interest = remaining_balance * r;    //    4.24
      if (remaining_balance < payment) {
         done = true ;
         payment = remaining_balance + interest;     //  848.65
         principle = payment - interest ;
         remaining_balance = 0.0 ;
      } else {
         principle = payment - interest ;
         remaining_balance -= principle ;
      }
      tprinciple += principle ;
      tinterest  += interest ;
      yprinciple += principle ;
      yinterest  += interest ;
      if ((++month % 12) == 0  ||  done) {
         printf("%3u    %9.2f  %9.2f  %9.2f\n", ++year, yprinciple, yinterest, remaining_balance) ;
         yprinciple = yinterest = 0.0 ;
      }
   }
   //  display totals paid
   puts  ("=====  =========  =========  =========");
   printf("total: %9.2f  %9.2f  %9.2f\n",
      tprinciple, tinterest, tprinciple + tinterest);
   printf("payoff in %u years, %u months\n", month / 12, month % 12) ;
}

//*****************************************************************
int main (int argc, char **argv)
{
   //  read command-line arguments
   if (argc != 5) {
      usage() ;
		return 1;
	}
   balance = strtod (argv[1], NULL);
   i = strtod (argv[2], NULL);
   N = strtod (argv[3], NULL);
   overpay = strtod (argv[4], NULL);
   r = (i / 100.0) / 12.0 ;         //  assume interest compounded monthly 

   if (overpay < 0) {
      usage() ;
      return 1;
   }

   if (N > 1.0) {
      compute_mortgage_payment() ;
   } else {
      compute_months_remaining() ;
   }

	return 0;
}
