### Win32 Source code snippets in C
<P>
<HR>
<FONT size=+1>
<LI>These are all small, typically command-line utilities that I've found
	to be useful.
<LI>All are compiled using the MinGW compiler package. This is an excellent,
*FREE* compiler package which is based on the GNU compiler, but links to Windows
libraries. &nbsp;MinGW is available from: 
<A href="http://www.mingw.org/"
target=_top>http://www.mingw.org/</A>
<LI>All are FREEWARE for any and all uses!!
</LI>
</FONT><BR><BR>

<table cellSpacing=0 cellPadding=0 width="100%" border=1>
  <tbody>
  <tr>
    <td bgColor=#FBEFD5>
       <CENTER><FONT color=#FBEFD5>
         T<SMALL>HE</SMALL>
         S<SMALL>OFTWARE</SMALL>
       </FONT></CENTER>
    </td>
  </tr>

  <tr>
    <td>
  <table>
    <tbody>
    <TR>
    <TD bgColor=#F5DEB3>
      <DIV class=h3 style="MARGIN-TOP: 0px; MARGIN-BOTTOM: 2px;
      COLOR: #205080; TEXT-ALIGN: left"><FONT size=+1>
      <CODE><B>ulocate</B></CODE> - This was initially written for use under 
			Linux, as a replacement for the obtuse <B><CODE>find</CODE></B> 
			command. &nbsp;It also works in Windows consoles. &nbsp;
      <B><CODE>ulocate</CODE></B> performs a case-insensitive search of all subdirectories below the 
			specified starting point for any filename containing the provided string.
			 &nbsp;Optionally, it can also search the PATH for the same criteria.
			</FONT></DIV>
      <DIV class=NEWS_FULL_SM>Last Update: <B>July 28, 2017</B></DIV>
    </TD></TR>

  <TR>
    <TD bgColor=#FBEFD5>
      <DIV class=t_news>
			<pre>
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
      </pre>
      Download <A href="ulocate.cpp">ulocate.cpp</A> here
    </DIV></TD></tr>
    </tbody>
  </table>
    </td>
  </tr>

  <tr>
    <td>
  <table>
    <tbody>
  <TR>
    <TD bgColor=#F5DEB3>
      <DIV class=h3
      style="MARGIN-TOP: 0px; MARGIN-BOTTOM: 2px; COLOR: #205080; TEXT-ALIGN: left"><FONT
      size=+1><CODE><B>printf2</B></CODE> - source code for <CODE>printf/sprintf</CODE>,
      with floating-point support, and no header-file dependencies</FONT></DIV>
      <DIV class=NEWS_FULL_SM>Last Update: <B>October 08, 2013</B> </DIV></TD></TR>
  <TR>
    <TD bgColor=#FBEFD5 colSpan=3>
			Embedded firmware environments provide a variety of challenges to the
			software developer. &nbsp;One of these challenges is that, typically,
			there is no "standard output" device to display messages on, so the
			archetypal <CODE>printf</CODE> function is not an option. &nbsp;However, the
			<CODE>sprintf</CODE> function is often still very useful for generating
			formatted strings.  Unfortunately, in most embedded environments, 
			linking stdio libraries is not an option, so a non-stdio version of 
			sprintf is very handy - but find such a function is <B><I>very</I></B> 
			challenging, especially if floating-point support is required!!<BR><BR>
			In 2002, Georges Menie distributed a version of 
			<A href="http://www.menie.org/georges/embedded/">
			<CODE>printf</CODE></A> 
			which had no <CODE>stdio/stdlib</CODE> dependencies, and seems to support all of the
			various obtuse <CODE>printf</CODE> syntaxes.
			 &nbsp;His code is clean, short, efficient, and builds on modern gcc
			 with almost no warnings!
			 &nbsp;Furthermore, it is distributed under
			LGPL, which means it can be used freely by anyone, for any purpose!
			 &nbsp;The only option missing from his function was floating-point
			support, which I need in my ARM9 implementation, so I added that
			capability to it. &nbsp;This version has no dependencies on any 
			standard headers or libraries.
      <BR><BR>
      Updates:<BR>
      <li>12/02/09 - fixed floating-point bug related to padding decimal
      portion with zeroes.<BR>
      <li>03/19/10 - pad fractional portion of floating-point number with 0s<BR>
      <li>07/20/10 - Fix a round-off bug in floating-point conversions<br>
                 ( 0.99 with %.1f did not round to 1.0 )
      <li>10/25/11 - Add support for %+ format (always show + on positive numbers)<BR>
      <li>01/19/12 - fix handling of %f with no decimal; it was defaulting 
				to 0 decimal places, rather than printf's 6.<BR>
      <li>05/10/13 - Add stringfn() function, which takes a maximum-output-buffer
          length as an argument.  Similar to snprintf()
      <li>09/28/13 - Fix bug in printchar(), which was using a fixed length test
           that was not appropriate for general use
      <li>10/08/13 - Add support for signed/unsigned long long (u64/i64)

Download <A href="printf2.c"> printf2.c</A> here
</DIV></TD></TR>
    </tbody>
  </table>
    </td>
  </tr>

  <tr>
    <td>
  <table>
    <tbody>
  <TR>
    <TD bgColor=#F5DEB3>
      <DIV class=h3
      style="MARGIN-TOP: 0px; MARGIN-BOTTOM: 2px; COLOR: #205080; TEXT-ALIGN: left"><FONT
      size=+1><CODE><B>serial_enum</B></CODE> - 
      Uses standard Windows/Win32 methods to enumerate serial devices
      </FONT></DIV>
      <DIV class=NEWS_FULL_SM>Last Update: <B>February 02, 2015</B> </DIV></TD></TR>
  <TR>
    <TD bgColor=#FBEFD5 colSpan=3>
      This is the Microsoft-approved method to enumerate all serial ports
      on a machine, including USB-serial devices. &nbsp;In addition to listing
      all the ports, it will report which ports can be opened (i.e., are 
      available for use).  Also added an option to display the Device Name,
      which can be passed to CreateFile().
      <BR><BR>
      Download 
      <A href="serial_enum.cpp">
      serial_enum.cpp</A> here
</DIV></TD></TR>
    </tbody>
  </table>
    </td>
  </tr>

  <tr>
    <td>
  <table>
    <tbody>
  <TR>
    <TD bgColor=#F5DEB3>
      <DIV class=h3
      style="MARGIN-TOP: 0px; MARGIN-BOTTOM: 2px; COLOR: #205080; TEXT-ALIGN: left"><FONT
      size=+1><CODE><B>rgb2cref</B></CODE> - 
      convert RGB value into hex/decimal form
      </FONT></DIV>
      <DIV class=NEWS_FULL_SM>Last Update: <B>July 26, 2020</B> </DIV></TD></TR>
  <TR>
    <TD bgColor=#FBEFD5 colSpan=3>
This is typically to aid in updating syntax-highlighting values for programming editors
      <BR><BR>
      Download 
      <A href="rgb2cref.cpp">
      rgb2cref.cpp</A> here
</DIV></TD></TR>
    </tbody>
  </table>
    </td>
  </tr>

  <tr>
    <td>
  <table>
    <tbody>
  <TR>
    <TD bgColor=#F5DEB3>
      <DIV class=h3
      style="MARGIN-TOP: 0px; MARGIN-BOTTOM: 2px; COLOR: #205080; TEXT-ALIGN: left"><FONT
      size=+1><CODE><B>hex_dump</B></CODE> - 
      Read data and display as both binary and ASCII data.
      </FONT></DIV>
      <DIV class=NEWS_FULL_SM>Last Update: <B>August 15, 2017</B> </DIV></TD></TR>
  <TR>
    <TD bgColor=#FBEFD5 colSpan=3>
      This displays a data file in standard hex-dump format, 
      comprising address + hex_data + ascii_data, looking like this:
      <pre>
007A0:  0C 89 5C 24 08 89 5C 24 04 C7 04 24 EE 30 40 00  | .?\$.?\$.?.$?0@. |
      </pre>
      Download <A href="hex_dump.cpp">hex_dump.cpp</A> here
</DIV></TD></TR>
    </tbody>
  </table>
    </td>
  </tr>

  <tr>
    <td>
  <table>
    <tbody>
  <TR>
    <TD bgColor=#F5DEB3>
      <DIV class=h3
      style="MARGIN-TOP: 0px; MARGIN-BOTTOM: 2px; COLOR: #205080; TEXT-ALIGN: left"><FONT
      size=+1><CODE><B>mrna2protein</B></CODE> - 
parse an mRNA nucleotide sequence, and output corresponding protein codes, in either 3 or 1 character format.
      </FONT></DIV>
      <DIV class=NEWS_FULL_SM>Last Update: <B>May 09, 2020</B> </DIV></TD></TR>
  <TR>
    <TD bgColor=#FBEFD5 colSpan=3>
      <pre>
Usage: mrna2protein -options CODON_SEQUENCE
Options:
-3 means output 3-character protein code (default)
-1 means output 1-character protein code
-t means assume template (3') DNA strand (default is sense (5') strand)

Note regarding CODON_SEQUENCE :
5' prefix and 3' suffix should *not* be included !!
Only [A,U,G,C] characters are valid
      </pre>
      Download <A href="mrna2protein.cpp">mrna2protein.cpp</A> here
</DIV></TD></TR>
    </tbody>
  </table>
    </td>
  </tr>

  <tr>
    <td>
  <table>
    <tbody>
  <TR>
    <TD bgColor=#F5DEB3>
      <DIV class=h3
      style="MARGIN-TOP: 0px; MARGIN-BOTTOM: 2px; COLOR: #205080; TEXT-ALIGN: left"><FONT
      size=+1><CODE><B>textfont</B></CODE> - 
      reads and displays MSDOS bit-mapped font files
      </FONT></DIV>
      <DIV class=NEWS_FULL_SM>Last Update: <B>October 04, 2017</B> </DIV></TD></TR>
  <TR>
    <TD bgColor=#FBEFD5 colSpan=3>
      reads and displays MSDOS bit-mapped font files.
      <BR><BR>
Download <A href="beer_cals.cpp">textfont.cpp</A> here
</DIV></TD></TR>
    </tbody>
  </table>
    </td>
  </tr>

  <tr>
    <td>
  <table>
    <tbody>
  <TR>
    <TD bgColor=#F5DEB3>
      <DIV class=h3
      style="MARGIN-TOP: 0px; MARGIN-BOTTOM: 2px; COLOR: #205080; TEXT-ALIGN: left"><FONT
      size=+1><CODE><B>beer_cals</B></CODE> - 
      Estimates calories of ale
      </FONT></DIV>
      <DIV class=NEWS_FULL_SM>Last Update: <B>September 22, 2015</B> </DIV></TD></TR>
  <TR>
    <TD bgColor=#FBEFD5 colSpan=3>
      Estimates calories of ale, from both alcohol and carbohydrates, 
      based upon volume and alcohol content.  Volume may be input in milliliters or ounces.
      <BR><BR>
      Download <A href="beer_cals.cpp">beer_cals.cpp</A> here
      </DIV></TD></TR>
    </tbody>
  </table>
    </td>
  </tr>

  <tr>
    <td>
  <table>
    <tbody>
  <TR>
    <TD bgColor=#F5DEB3>
      <DIV class=h3
      style="MARGIN-TOP: 0px; MARGIN-BOTTOM: 2px; COLOR: #205080; TEXT-ALIGN: left"><FONT
      size=+1><CODE><B>readall</B></CODE> - 
      list all files specified by a filespec with wildcards
      </FONT></DIV>
      <DIV class=NEWS_FULL_SM>Last Update: <B>October 12, 2020</B> </DIV></TD></TR>
  <TR>
    <TD bgColor=#FBEFD5 colSpan=3>
      list all files specified by a filespec with wildcards
      <BR><BR>
      Download <A href="readall.cpp">readall.cpp</A> here
      </DIV></TD></TR>
    </tbody>
  </table>
    </td>
  </tr>

  <tr>
    <td>
  <table>
    <tbody>
  <TR>
    <TD bgColor=#F5DEB3>
      <DIV class=h3
      style="MARGIN-TOP: 0px; MARGIN-BOTTOM: 2px; COLOR: #205080; TEXT-ALIGN: left"><FONT
      size=+1><CODE><B>ascii</B></CODE> - 
      Displays an ASCII table, in decimal, hex and ASCII, for all 256 characters
      </FONT></DIV>
      <DIV class=NEWS_FULL_SM>Last Update: <B>August 10, 2017</B> </DIV></TD></TR>
  <TR>
    <TD bgColor=#FBEFD5 colSpan=3>
      <BR>
      <CENTER>Download 
      <A href="ascii.cpp">
      ascii.cpp</A> here
      </CENTER></DIV></TD>
      </TR>
    </tbody>
  </table>
    </td>
  </tr>

  <tr>
    <td>
  <table>
    <tbody>
  <TR>
    <TD bgColor=#F5DEB3>
      <DIV class=h3
      style="MARGIN-TOP: 0px; MARGIN-BOTTOM: 2px; COLOR: #205080; TEXT-ALIGN: left"><FONT
      size=+1><CODE><B>apptest</B></CODE> - 
      This inappropriately-named file demonstrates two common WinAPI tasks
      </FONT></DIV>
      <DIV class=NEWS_FULL_SM>Last Update: <B>June 24, 2018</B> </DIV></TD></TR>
  <TR>
    <TD bgColor=#FBEFD5 colSpan=3>
			This console application demonstrates two common WinAPI tasks:
			<li>Use printf() to output UNICODE messages to the console
			<li>Display various Windows system/user directories
      <BR><BR>
      Download <A href="apptest.cpp">apptest.cpp</A> here
      </DIV></TD></TR>
    </tbody>
  </table>
    </td>
  </tr>

  <tr>
    <td>
  <table>
    <tbody>
  <TR>
    <TD bgColor=#F5DEB3>
      <DIV class=h3
      style="MARGIN-TOP: 0px; MARGIN-BOTTOM: 2px; COLOR: #205080; TEXT-ALIGN: left"><FONT
      size=+1><CODE><B>dms2dd</B></CODE> - 
      Convert between degrees/minutes/seconds and decimal degrees
      </FONT></DIV>
      <DIV class=NEWS_FULL_SM>Last Update: <B>September 30, 2010</B> </DIV></TD></TR>
  <TR>
    <TD bgColor=#FBEFD5 colSpan=3>
Usage: dms2dd input_value<br>
Enter dms as degrees.minutes.seconds<br>
Enter dd as integer_degrees.fractional_degrees<br><br>
If dms2dd sees two decimal points in the input,<br>
the value will be treated as dms, and dd will be calculated and output.<br><br>
If dms2dd sees one decimal points in the input,<br>
the value will be treated as dd, and dms will be calculated and output.<br>
      <BR>
      Download <A href="dms2dd.cpp">dms2dd.cpp</A> here
</DIV></TD></TR>
    </tbody>
  </table>
    </td>
  </tr>

  <tr>
    <td>
  <table>
    <tbody>
  <TR>
    <TD bgColor=#F5DEB3>
      <DIV class=h3
      style="MARGIN-TOP: 0px; MARGIN-BOTTOM: 2px; COLOR: #205080; TEXT-ALIGN: left"><FONT
      size=+1><CODE><B>mortgage</B></CODE> - 
      Performs mortgage calculations
      </FONT></DIV>
      <DIV class=NEWS_FULL_SM>Last Update: <B>December 23, 2011</B> </DIV></TD></TR>
  <TR>
    <TD bgColor=#FBEFD5 colSpan=3>
Usage: MORTGAGE principle interest number_months overpay
In normal operation, this program computes the monthly payment,
and payment summary table, for a loan with the input parameters.
<br><br>
Alternate mode:
If number_months is 0, overpay is treated as the payment amount,
and this program will // calculate and display the number of months
required to pay off principle.
<br><br>
Also, if overpay is greater than payment, then overpay is treated as
total payment amount, and overpay is calculated from that.
      <BR><BR>
      Download <A href="mortgage.cpp">mortgage.cpp</A> here
      </DIV></TD></TR>
    </tbody>
  </table>
    </td>
  </tr>

  <tr>
    <td>
  <table>
    <tbody>
  <TR>
    <TD bgColor=#F5DEB3>
      <DIV class=h3
      style="MARGIN-TOP: 0px; MARGIN-BOTTOM: 2px; COLOR: #205080; TEXT-ALIGN: left"><FONT
      size=+1><CODE><B>prime32</B></CODE> - 
      calculates factors of a number, or reports if input is prime
      </FONT></DIV>
      <DIV class=NEWS_FULL_SM>Last Update: <B>February 22, 2012</B> </DIV></TD></TR>
  <TR>
    <TD bgColor=#FBEFD5 colSpan=3>
PRIME32.EXE - Written by: Daniel D. Miller<br>
****************************************************<br>
This program determines whether a number is a prime,<br>
then displays either the number or its factors.<br>
      <BR>
      Download <A href="prime32.cpp">prime32.cpp</A> here
      </DIV></TD></TR>
    </tbody>
  </table>
    </td>
  </tr>

  <tr>
    <td>
    <table>
    <tbody>
  <TR>
    <TD bgColor=#F5DEB3>
      <DIV
      style="MARGIN-TOP: 0px; MARGIN-BOTTOM: 2px; COLOR: #205080; TEXT-ALIGN: left">
      <FONT size=+1>C/C++ makefile-dependency generator </FONT></DIV>
      <DIV>Last Update: <B>February 02, 2007</B> </DIV></TD></TR>
  <TR>
    <TD bgColor=#FBEFD5 colSpan=3>
      <DIV>
      This is the version of makedepend which is distributed with Xfree86.
      &nbsp;It is <b>Copyright (c) 1993, 1994, 1998 The Open Group</b> and has
      a free-distribution message in each source file. &nbsp;I made a couple of
      trivial changes to the source to make it compile with modern gcc,
      but otherwise it still works superbly and needs no changes.
      <BR>
      To use this utility, make the following changes to your makefile:
      <BR><BR>
      1. Add this line to the end of the makefile:<pre>
# DO NOT DELETE</pre>
      2. Add a rule for running makedepend:<pre>
SRCS=file1.cpp file2.cpp file3.cpp
depend:
  makedepend -I. $(SRCS)
</pre>
      3. run <pre>make depend</pre> to update the dependencies. &nbsp;
      That's it!!  Your makefile now has the appropriate dependency list.
      <BR><BR>
      <A href="makedepend.src.zip">
      Download the source code package here</A>
      </DIV></TD></TR>
    </tbody>
  </table>
    </td>
  </tr>
  </tbody>
</table>

