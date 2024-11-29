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

//****************************************************************************
uint calc_elapsed_time(bool done)
{
   static u64 ti = 0 ;
   uint secs = 0 ;
   if (!done) {
      ti = proc_time() ;
   } else {
      u64 tf = proc_time() ;
      secs = (uint) ((tf - ti) / get_clocks_per_second()) ;
      // syslog("send_serial_msg: %u seconds", secs) ;
   }
   return secs;
}

//****************************************************************************
uint calc_elapsed_msec(bool done)
{
   static u64 ti = 0 ;
   uint secs = 0 ;
   if (!done) {
      ti = proc_time() ;
   } else {
      u64 tf = proc_time() ;
      secs = (uint) ((tf - ti) / (get_clocks_per_second()/1000)) ;
      // syslog("send_serial_msg: %u seconds", secs) ;
   }
   return secs;
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

