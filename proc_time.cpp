//  Measure elapsed time using QueryPerformanceCounter()
//  build: g++ -Wall -O2 proc_time.cpp -o proc_time.exe

#include <windows.h>
#include <stdio.h>

typedef unsigned int          uint ;
typedef unsigned long long    u64 ;

//*****************************************************************************
u64 proc_time(void)
{
   // return (unsigned) clock() ;
   LARGE_INTEGER ti ;
   QueryPerformanceCounter(&ti) ;
   return (u64) ti.QuadPart ;
}

//*************************************************************************
u64 get_clocks_per_second(void)
{
   static u64 clocks_per_sec64 = 0 ;
   if (clocks_per_sec64 == 0) {
      LARGE_INTEGER tfreq ;
      QueryPerformanceFrequency(&tfreq) ;
      clocks_per_sec64 = (u64) tfreq.QuadPart ;
   }
   return clocks_per_sec64 ;
}

//*************************************************************************
u64 get_clocks_per_msec(void)
{
   return get_clocks_per_second() / 1000 ;
}

//****************************************************************************
u64 calc_elapsed_time(bool done)
{
   static u64 ti = 0 ;
   u64 secs = 0 ;
   if (!done) {
      ti = proc_time() ;
   } else {
      u64 tf = proc_time() ;
      secs = (tf - ti) / get_clocks_per_second() ;
   }
   return secs;
}

//****************************************************************************
u64 calc_elapsed_msec(bool done)
{
   static u64 ti = 0 ;
   u64 msecs = 0 ;
   if (!done) {
      ti = proc_time() ;
   } else {
      u64 tf = proc_time() ;
      msecs = (tf - ti) / get_clocks_per_msec() ;
   }
   return msecs;
}

//****************************************************************************
int main(void)
{
   printf("measuring time via QueryPerformanceCounter()\n");
   calc_elapsed_msec(false);  //  initialize counter
   SleepEx(2000, false);
   uint msecs = calc_elapsed_msec(true);
   printf("Elapsed time: %u msecs\n", msecs);
   return 0;
}

