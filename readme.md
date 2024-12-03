## Win32 Source code snippets in C

- These are all small, typically command-line utilities that I have found
   to be useful.
- All are compiled using the MinGW compiler package. This is an excellent,
*FREE* compiler package which is based on the GNU compiler, but links to Windows
libraries. I recommend the [TDM](http://tdm-gcc.tdragon.net/) distribution, 
to avoid certain issues with library accessibility. 

NOTE: from September 2022, some of the files in this folder are built with
64-bit compiler, in order to support functionality in certain utilities.

- All programs here are licensed under Creative Commons CC0 1.0 Universal;  
https://creativecommons.org/publicdomain/zero/1.0/

*** 

__ulocate__ - This was initially written for use under 
   Linux, as a replacement for the obtuse `find` 
   command. It also works in Windows consoles. 
__ulocate__ performs a case-insensitive search of all subdirectories below the 
   specified starting point for any filename containing the provided string.
     Optionally, it can also search the PATH for the same criteria.  
Last Update: __July 28, 2017__

``` 
ULOCATE.EXE, Version 1.15
Usage: ulocate [options] name_component start_path
This program displays all filenames containing a specified
name component, starting at directory [start_path].
Default start_path is current location.

output-format options:
 -d  show file date
 -t  show file time
 -s  show file size (short format)
 -S  show file size (byte format)
     -xl  l = width of size field for -S (default = 6)
 -n  show file name (default)

other options:
 -v  Verbose mode - show search paths
 -b  Debug mode - show other process information
 -l  follow symbolic links
 -w  match exact string (whole-word search)
 -/  Use backslash vs forward slash for path elements

 -p  Search for name_component in the directories in the PATH variable
     NOTE: This option replaces the normal ulocate functionality with
     alternate functionality.  In this mode, subdirectories are NOT searched!!

 Options can also be placed in the ULOCATE environment variable
```  
*** 

__heron__ - This program implements Heron's Formula,
which calculates the area of an irregular triangle,
given only the lengths of the three sides, but no angles.  
Last Update: __February 10, 2023__

``` 
Usage: heron side1_len side2_len side3_len

This program implements Heron's Formula,
which calculates the area of an irregular triangle,
given only the lengths of the three sides, but no angles.

parameters: side1: 950.000, side2: 510.000, side3: 1200.000
intermediate sum parameter: 1330.000
area: 232111.266
```   
*** 

__printf2__ - source code for `printf/sprintf`, with floating-point support, 
and no header-file dependencies, for use in embedded projects.  
Last Update: __October 08, 2013__

Embedded firmware environments provide a variety of challenges to the
software developer. &nbsp;One of these challenges is that, typically,
there is no "standard output" device to display messages on, so the
archetypal `printf` function is not an option. &nbsp;However, the
`sprintf` function is often still very useful for generating
formatted strings.  Unfortunately, in most embedded environments, 
linking stdio libraries is not an option, so a non-stdio version of 
sprintf is very handy - but find such a function is ___very___ 
challenging, especially if floating-point support is required!!
In 2002, Georges Menie distributed a version of 
[printf](http://www.menie.org/georges/embedded/)
which had no `stdio/stdlib` dependencies, and seems to support all of the
various obtuse `printf` syntaxes.
 &nbsp;His code is clean, short, efficient, and builds on modern gcc
 with almost no warnings!
 &nbsp;Furthermore, it is distributed under
LGPL, which means it can be used freely by anyone, for any purpose!
 &nbsp;The only option missing from his function was floating-point
support, which I need in my ARM9 implementation, so I added that
capability to it. &nbsp;This version has no dependencies on any 
standard headers or libraries.

Updates:
- 12/02/09 - fixed floating-point bug related to padding decimal
portion with zeroes.
- 03/19/10 - pad fractional portion of floating-point number with 0s
- 07/20/10 - Fix a round-off bug in floating-point conversions
           ( 0.99 with %.1f did not round to 1.0 )
- 10/25/11 - Add support for %+ format (always show + on positive numbers)
- 01/19/12 - fix handling of %f with no decimal; it was defaulting 
      to 0 decimal places, rather than 6 in printf.  
- 05/10/13 - Add `stringfn()` function, which takes a maximum-output-buffer
    length as an argument.  Similar to `snprintf()`
- 09/28/13 - Fix bug in printchar(), which was using a fixed length test
     that was not appropriate for general use
- 10/08/13 - Add support for signed/unsigned long long (u64/i64)

*** 

__serial_enum__ - Uses standard Windows/Win32 methods to enumerate serial devices  
Last Update: __February 02, 2015__

This is the Microsoft-approved method to enumerate all serial ports
on a machine, including USB-serial devices. &nbsp;In addition to listing
all the ports, it will report which ports can be opened (i.e., are 
available for use).  Also added an option to display the Device Name,
which can be passed to `CreateFile()`.

*** 

__proc_time__ - Measure elapsed time using QueryPerformanceCounter()  
Last Update: __November 29, 2024__

Measure elapsed time using QueryPerformanceCounter() on Windows systems

*** 

__rgb2cref__ - convert RGB value into hex/decimal form  
Last Update: __July 26, 2020__

This is typically to aid in updating syntax-highlighting values for programming editors

*** 

__cline__ - generate argc/argv data from command-line input  
Last Update: __July 12, 2024__

How to generate argc/argv data for programs which do not have conventional main() function.  
These include Windows (WinMain) and embedded projects.

```
D:\SourceCode\Git\snippets > cline derelict history*
argv[0] = cline
argc=3
1: derelict
2: history*
```


*** 

__wcmdline__ - Unicode command-line handling and console output  
Last Update: __July 13, 2024__

Handle command-line arguments and console output for UNICODE programs.
```
D:\SourceCode\Git\snippets > wcmdline test 1 2 3
home=D:\SourceCode\Git\snippets
argc=5
arg 1: test
arg 2: 1
arg 3: 2
arg 4: 3
```

*** 

__hex_dump__ - Read data and display as both binary and ASCII data  
Last Update: __August 15, 2017__

This displays a data file in standard hex-dump format, 
comprising address + hex_data + ascii_data, looking like this:
```
007A0:  0C 89 5C 24 08 89 5C 24 04 C7 04 24 EE 30 40 00  | .?\$.?\$.?.$?0@. |
```

*** 

__mrna2protein__ - parse an mRNA nucleotide sequence, and output corresponding 
protein codes, in either 3 or 1 character format.  
Last Update: __May 09, 2020__

```
Usage: mrna2protein -options CODON_SEQUENCE
Options:
-3 means output 3-character protein code (default)
-1 means output 1-character protein code
-t means assume template (3') DNA strand (default is sense (5') strand)

Note regarding CODON_SEQUENCE :
5' prefix and 3' suffix should *not* be included !!
Only [A,U,G,C] characters are valid
```

*** 

__readall__ - list all files specified by a filespec with wildcards  
Last Update: __October 12, 2020__

List all files specified by a filespec with wildcards.  
This can be used as a template file for reading text files and
doing various tasks.

*** 

__ascii__ - Displays an ASCII table, in decimal, hex and ASCII, for all 256 characters  
Last Update: __August 10, 2017__

*** 

__apptest__ - This inappropriately-named file demonstrates two common WinAPI tasks  
Last Update: __June 24, 2018__

This console application demonstrates two common WinAPI tasks:
- Use printf() to output UNICODE messages to the console
- Display various Windows system/user directories

*** 

__dms2dd__ - Convert between degrees/minutes/seconds and decimal degrees  
Last Update: __September 30, 2010__

```
Usage: dms2dd input_value  
Enter dms as degrees.minutes.seconds  
Enter dd as integer_degrees.fractional_degrees  
If dms2dd sees two decimal points in the input, 
the value will be treated as dms, and dd will be calculated and output.  
If dms2dd sees one decimal points in the input, 
the value will be treated as dd, and dms will be calculated and output. 
```
       
*** 

__mortgage__ - Performs mortgage calculations  
Last Update: __December 23, 2011__

```
Usage: MORTGAGE principle interest number_months overpay
```
In normal operation, this program computes the monthly payment,
and payment summary table, for a loan with the input parameters.
  
Alternate mode:
If number_months is 0, overpay is treated as the payment amount,
and this program will calculate and display the number of months
required to pay off principle.
  
Also, if overpay is greater than payment, then overpay is treated as
total payment amount, and overpay is calculated from that.
        
*** 

__prime64__ (formerly __prime32__)- calculates factors of a number, or reports if input is prime  
Last Update: __October 24, 2022__

PRIME64.EXE - Written by: Daniel D. Miller  
This program determines whether a number is a prime, 
then displays either the number or its factors. 

Note: converting to 64-bit build, sped up handling of number which had
a factor that was greater than 32 bits, by about 25%.
       
*** 
__beer_cals__ - Estimates calories of ale  
Last Update: __September 22, 2015__

Estimates calories of ale, from both alcohol and carbohydrates, 
based upon volume and alcohol content.  Volume may be input in milliliters or ounces.
Carbohydrates are calculated at 4 grams/ml of ale, which is not at all a good approximation,
but I don't know any better method.

*** 

__makedepend__ - C/C++ makefile-dependency generator  
__February 02, 2007__

This is the version of makedepend which is distributed with Xfree86.
&nbsp;It is __Copyright (c) 1993, 1994, 1998 The Open Group__ and has
a free-distribution message in each source file. &nbsp;I made a couple of
trivial changes to the source to make it compile with modern gcc,
but otherwise it still works superbly and needs no changes.
 
To use this utility, make the following changes to your makefile:
  
1. Add this line to the end of the makefile:<pre>
`# DO NOT DELETE`
2. Add a rule for running makedepend:<pre>
```
SRCS=file1.cpp file2.cpp file3.cpp
depend:
  makedepend -I. $(SRCS)
```

3. run <pre>make depend</pre> to update the dependencies.  

That is it!!  Your makefile now has the appropriate dependency list.
