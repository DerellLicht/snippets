//************************************************************************* 
// Serial port enumeration routines
// 
// The EnumSerialPort function will populate an array of SSerInfo structs,
// each of which contains information about one serial port present in
// the system. Note that this code must be linked with setupapi.lib,
// which is included with the Win32 SDK.
// 
// by Zach Gorman <gormanjz@hotmail.com>
// 
// Copyright (c) 2002 Archetype Auction Software, Inc. All rights reserved.
// 
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following condition is
// met: Redistributions of source code must retain the above copyright
// notice, this condition and the following disclaimer.
// 
// THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESSED OR IMPLIED
// WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
// OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
// DISCLAIMED. IN NO EVENT SHALL ARCHETYPE AUCTION SOFTWARE OR ITS
// AFFILIATES BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// 
//*************************************************************************
// Copyright (c) 2003-2015  Daniel D Miller
// This version of this program is distributed by me as free software,
// under the same terms as Zach Gorman's original version;
// see the license listed above.
//  
// compile with:  g++ -Wall -O2 -s -DUNICODE -D_UNICODE serial_enum.cpp -o serial_enum.exe -lsetupapi
// ************************************************************************
//  DDM: lessons learned:
//  strFriendlyName contains Windows name/description of port
//  strDevPath is the path which is used for accessing the port
//     (via CreateFile, etc)
// ************************************************************************

#define  STAND_ALONE    1
#include <windows.h>
#include <tchar.h>
#ifdef  STAND_ALONE
#include <stdio.h>
#endif
#ifdef _lint
#include <stdlib.h>  //  atoi()
#endif
#include <limits.h>
#include <ctype.h>

// #include "wcommon.h"

#define  MAX_PATH_LEN   1024

// The following includes are needed for serial port enumeration
#include <initguid.h>   //  other GUIDs
#include <Setupapi.h>   //  SP_DEVICE_INTERFACE_DETAIL_DATA

// The following define is from ntddser.h in the DDK. It is also
// needed for serial port enumeration.
//lint -esym(765, GUID_CLASS_COMPORT, GUID_DEVINTERFACE_COMPORT)

#ifndef GUID_CLASS_COMPORT
DEFINE_GUID(GUID_CLASS_COMPORT, 0x86e0d1e0L, 0x8089, 0x11d0, 0x9c, 0xe4, 0x08, 0x00, 0x3e, 0x30, 0x1f, 0x73);
#endif

#ifndef GUID_DEVINTERFACE_COMPORT
DEFINE_GUID(GUID_DEVINTERFACE_COMPORT, 0x86E0D1E0L, 0x8089, 0x11D0, 0x9C, 0xE4, 0x08, 0x00, 0x3E, 0x30, 0x1F, 0x73);
#endif

// Struct used when enumerating the available serial ports
// Holds information about an individual serial port.
typedef struct SSerInfo_s {
   struct SSerInfo_s *next ;
   unsigned index ;
   unsigned portnum ;
   TCHAR strDevPath[MAX_PATH_LEN+1];          // Device path for use with CreateFile()
   TCHAR strFriendlyName[MAX_PATH_LEN+1];     // Full name to be displayed to a user
   BOOL bDeviceOpens;
   BOOL bEnumerates;
} SSerInfo_t, *SSerInfo_p ;
static SSerInfo_p sp_top = 0 ;
static SSerInfo_p sp_tail ;
static unsigned port_count = 0 ;

#ifdef  STAND_ALONE
//**********************************************************************
void strip_newlines(char *rstr)
{
   int slen = (int) strlen(rstr) ;
   while (1) {
      if (slen == 0)
         break;
      if (*(rstr+slen-1) == '\n'  ||  *(rstr+slen-1) == '\r') {
         slen-- ;
         *(rstr+slen) = 0 ;
      } else {
         break;
      }
   }
}

//*************************************************************
//  each subsequent call to this function overwrites
//  the previous report.
//*************************************************************
char *get_system_message(void)
{
   static char msg[261] ;
   // int slen ;

   LPVOID lpMsgBuf;
   FormatMessage(
      FORMAT_MESSAGE_ALLOCATE_BUFFER |
      FORMAT_MESSAGE_FROM_SYSTEM |
      FORMAT_MESSAGE_IGNORE_INSERTS,
      NULL,
      GetLastError(),
      MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
      (LPTSTR) &lpMsgBuf,
      0, 0);
   // Process any inserts in lpMsgBuf.
   // ...
   // Display the string.
   strncpy(msg, (char *) lpMsgBuf, 260) ;

   // Free the buffer.
   LocalFree( lpMsgBuf );

   //  trim the newline off the message before copying it...
   strip_newlines(msg) ;
   // slen = strlen(msg) ;
   // if (msg[slen-1] == 10  ||  msg[slen-1] == 13) {
   //    msg[slen-1] = 0 ;
   // }

   return msg;
}

//********************************************************************
//  On Windows platform, try to redefine printf/fprintf
//  so we can output code to a debug window.
//  Also, shadow syslog() within OutputDebugString()
//  Note: printf() remapping was unreliable,
//  but syslog worked great.
//********************************************************************
int syslog(const char *fmt, ...)
{
   char consoleBuffer[3000] ;
   va_list al; //lint !e522

   va_start(al, fmt);   //lint !e1055 !e530
   vsprintf(consoleBuffer, fmt, al);   //lint !e64
   // if (common_logging_enabled)
   //    fprintf(cmlogfd, "%s", consoleBuffer) ;
   puts(consoleBuffer) ;
   va_end(al);
   return 1;
}

#endif
//****************************************************************************
static bool port_enumerated(unsigned new_port)
{
   SSerInfo_p sptemp ;
   for (sptemp = sp_top; sptemp != 0; sptemp = sptemp->next) {
      if (sptemp->portnum == new_port)
         return true;
   }
   return false;
}

//****************************************************************************
void add_new_port(unsigned new_port)
{
   SSerInfo_p si = new SSerInfo_t ;
   memset((char *) si, 0, sizeof(SSerInfo_t)) ;
   wsprintf(si->strDevPath, _T("\\\\.\\COM%u"), new_port) ;
   wsprintf(si->strFriendlyName, _T("unknown device (COM%u)"), new_port) ;
   si->portnum = new_port ;
   si->bDeviceOpens = 1 ;
   si->index = port_count++ ;
   
   //  add new struct to list
   if (sp_top == 0) 
      sp_top = si ;
   else 
      sp_tail->next = si ;
   sp_tail = si ;
}

//****************************************************************************
void add_new_port(unsigned new_port, TCHAR *pszFriendlyName)
{
   SSerInfo_p si = new SSerInfo_t ;
   memset((char *) si, 0, sizeof(SSerInfo_t)) ;
   // wsprintf(si->strDevPath, "COM%u", new_port) ;
   wsprintf(si->strDevPath, _T("\\\\.\\COM%u"), new_port) ;
   // wsprintf(si->strFriendlyName, "unknown device (COM%u)", new_port) ;
   _tcscpy(si->strFriendlyName, pszFriendlyName) ;
   si->portnum = new_port ;
   si->bDeviceOpens = 1 ;
   si->index = port_count++ ;
   
   //  add new struct to list
   if (sp_top == 0) 
      sp_top = si ;
   else 
      sp_tail->next = si ;
   sp_tail = si ;
}

//****************************************************************************
//  Eldad Kuperman 5:27 13 Apr '00
//  I'm using the following code for enumeration. 
//  Tested succesfuly on Win98, NT and 2000. Hope it will work for you.
//  No CreateFile/CloseHandle is needed.
//  
//  PJ Naughter 20:00 16 May '00
//  Nice one. I'll update the source ASAP. 
//  One thing I might do is use the format "\.\COMX" instead 
//  of "COMx" as Windows 9x cannot handle the latter format 
//  for ports with a number greater than 9. 
//  
//  DDM  07/15/09 09:39
//  Well, \\.\\COMx did not work under Vista
//  
//****************************************************************************
static bool comm_port_avail[256] ;

//****************************************************************************
BOOL IsPortAvailable(int nPort)
{
   TCHAR szPort[15];
   COMMCONFIG cc;
   DWORD dwCCSize;

   wsprintf(szPort, _T("COM%d"), nPort);

   // Check if this port is available
   dwCCSize = sizeof(cc);
   return GetDefaultCommConfig(szPort, &cc, &dwCCSize);
}

//****************************************************************************
//  Unlike EnumSerialPorts(), this function turns up the dialup modem,
//  which isn't entirely desireable.  However, it also turns up the
//  Belkin USB-Serial Adapter, which the former function did not.
//****************************************************************************
void CountCommPorts(void)
{
   unsigned idx ;
   for (idx=0; idx<256; idx++)
      comm_port_avail[idx] = false ;
   // ports.RemoveAll();
   for (idx = 1; idx <= 255; idx++) {
      if (IsPortAvailable( idx )) {
         comm_port_avail[idx] = true ;
         // printf("found COM%u\n", idx) ;
         // ports.Add( nPort );
         if (!port_enumerated(idx)) 
            add_new_port(idx) ;
      }
   }
}

//****************************************************************************
//lint -esym(714, get_serport_count)
//lint -esym(759, get_serport_count)
//lint -esym(765, get_serport_count)
unsigned get_serport_count(void)
{
   return port_count;
}

//****************************************************************************
//  note that 0 is not a valid COMM port number, 
//  so return value of 0 indicates failure.
//****************************************************************************
unsigned get_comm_num(unsigned index)
{
   SSerInfo_p si ;
   // printf("seeking %s\n", pszFriendlyName) ;
   for (si=sp_top; si != 0; si = si->next) {
      if (si->index == index)
         return si->portnum;
   }
   return 0;
}

//****************************************************************************
int get_comm_index(unsigned port_num)
{
   SSerInfo_p si ;
   // printf("seeking %s\n", pszFriendlyName) ;
   for (si=sp_top; si != 0; si = si->next) {
      if (si->portnum == port_num)
         // return si->portnum;
         return si->index;
   }
   return 0;
}

//****************************************************************************
TCHAR *get_dev_path(unsigned port_num)
{
   SSerInfo_p si ;
   // printf("seeking %s\n", pszFriendlyName) ;
   for (si=sp_top; si != 0; si = si->next) {
      if (si->portnum == port_num)
         // return si->portnum;
         return si->strDevPath ;
   }
   return 0;
}

//****************************************************************************
TCHAR *get_disp_path(unsigned port_num)
{
   SSerInfo_p si ;
   // printf("seeking %s\n", pszFriendlyName) ;
   for (si=sp_top; si != 0; si = si->next) {
      if (si->portnum == port_num)
         // return si->portnum;
         return si->strFriendlyName ;
   }
   return 0;
}

//****************************************************************************
static bool match_port_number(TCHAR *pszFriendlyName, unsigned nPort)
{
   SSerInfo_p si ;
   // printf("seeking %s\n", pszFriendlyName) ;
   for (si=sp_top; si != 0; si = si->next) {
      // if (si->portnum != 0)
      //    continue;
      // printf("%s=%s\n", si->strPortDesc, pszFriendlyName);
      if (_tcscmp(si->strFriendlyName, pszFriendlyName) == 0) {
         si->portnum = nPort ;
         return true;
      }
   }
   return false;
}

//***********************************************************************
void fill_cport_combobox(HWND hwnd, unsigned init_idx)
{
   TCHAR cbentry[81] ;

   SSerInfo_p sptemp ;
   // unsigned j ;
   // for (j=0; elements[j] != 0; j++) {
   for (sptemp = sp_top; sptemp != 0; sptemp = sptemp->next) {
      // printf("%s\n", sptemp->strDevPath) ;
      wsprintf(cbentry, _T("COM%u: %s"), sptemp->portnum, sptemp->strFriendlyName ) ;
      
      // wsprintf(msgstr, "adding %s\n", items[j]) ;
      // OutputDebugString(msgstr) ;
      LRESULT result = SendMessage(hwnd, CB_ADDSTRING, 0, (LPARAM) cbentry);
      switch (result) {
      case CB_ERR:
         syslog("CB_ADDSTRING: CB_ERR: %s\n", get_system_message()) ;
         break;
      case CB_ERRSPACE:
         syslog("CB_ADDSTRING: CB_ERRSPACE: %s\n", get_system_message()) ;
         break;

      default:
         // wsprintf(msgstr, "CB_ADDSTRING returned %u\n", result) ;
         // OutputDebugString(msgstr) ;
         break;
      }
   }
   SendMessage(hwnd, CB_SETCURSEL, (WPARAM) init_idx, 0);
}

//****************************************************************************
BOOL IsNumeric(LPCTSTR pszString, BOOL bIgnoreColon)
{
  size_t nLen = _tcslen(pszString);
  if (nLen == 0)
    return FALSE;

  //Assume the best
  BOOL bNumeric = TRUE;

  for (size_t i=0; i<nLen && bNumeric; i++)
  {
    bNumeric = (_istdigit(pszString[i]) != 0);
    if (bIgnoreColon && (pszString[i] == _T(':')))
      bNumeric = TRUE;
  }

  return bNumeric;
}

//****************************************************************************
//  from: http://www.naughter.com/enumser.html  
//****************************************************************************
// BOOL UsingSetupAPI2(unsigned *ports)
BOOL UsingSetupAPI2(void)
{
  //First need to convert the name "Ports" to a GUID using SetupDiClassGuidsFromName
  DWORD dwGuids = 0;
  // lpfnSETUPDICLASSGUIDSFROMNAME(_T("Ports"), NULL, 0, &dwGuids);
  SetupDiClassGuidsFromName(_T("Ports"), NULL, 0, &dwGuids) ;
  if (dwGuids == 0) {
      printf("SetupDiClassGuidsFromName (init): %s\n", get_system_message()) ;
      return FALSE;
  }
  // printf("guids=%u\n", (unsigned) dwGuids) ;

  //Allocate the needed memory
  GUID *pGuids = new GUID[dwGuids] ;
  // ATL::CHeapPtr<GUID> pGuids;
  // if (!pGuids.Allocate(dwGuids))
  if (pGuids == NULL) {
      SetLastError(ERROR_OUTOFMEMORY);
      puts("Out Of Memory") ;
      return FALSE;
  }

  //Call the function again
  if (!SetupDiClassGuidsFromName(_T("Ports"), pGuids, dwGuids, &dwGuids)) {
      printf("SetupDiClassGuidsFromName (read): %s\n", get_system_message()) ;
      return FALSE;
  }

  //Now create a "device information set" which is required to enumerate all the ports
  HDEVINFO hDevInfoSet = SetupDiGetClassDevs(pGuids, NULL, NULL, DIGCF_PRESENT);
  if (hDevInfoSet == INVALID_HANDLE_VALUE) {
      printf("SetupDiGetClassDevs: %s\n", get_system_message()) ;
      return FALSE;
  }

  //Finally do the enumeration
  BOOL bMoreItems = TRUE;
  int nIndex = 0;
  SP_DEVINFO_DATA devInfo;
  while (bMoreItems)
  {
    //Enumerate the current device
    devInfo.cbSize = sizeof(SP_DEVINFO_DATA);
    // bMoreItems = lpfnSETUPDIENUMDEVICEINFO(hDevInfoSet, nIndex, &devInfo);
    bMoreItems = SetupDiEnumDeviceInfo(hDevInfoSet, nIndex, &devInfo);
    if (bMoreItems)
    {
      //Did we find a serial port for this device
      BOOL bAdded = FALSE;
      unsigned nPort = 0 ;

      //Get the registry key which stores the ports settings
      HKEY hDeviceKey = SetupDiOpenDevRegKey(hDevInfoSet, &devInfo, DICS_FLAG_GLOBAL, 0, DIREG_DEV, KEY_QUERY_VALUE);
      if (hDeviceKey)
      {
        //Read in the name of the port
        TCHAR pszPortName[256];
        DWORD dwSize = sizeof(pszPortName);
        DWORD dwType = 0;
       if ((RegQueryValueEx(hDeviceKey, _T("PortName"), NULL, &dwType, reinterpret_cast<LPBYTE>(pszPortName), &dwSize) == ERROR_SUCCESS) && (dwType == REG_SZ))
        {
          //If it looks like "COMX" then
          //add it to the array which will be returned
          size_t nLen = _tcslen(pszPortName);
          if (nLen > 3)
          {
            if ((_tcsnicmp(pszPortName, _T("COM"), 3) == 0) && IsNumeric(&pszPortName[3], FALSE))
            {
              //Work out the port number
              nPort = _ttoi(&pszPortName[3]);
              // ports.Add(nPort);
              // printf("add port %u\n", nPort) ;
              bAdded = TRUE;
            }
          }
        }

        //Close the key now that we are finished with it
        RegCloseKey(hDeviceKey);
      }

      //If the port was a serial port, then also try to get its friendly name
      if (bAdded)
      {
        TCHAR pszFriendlyName[256];
        DWORD dwSize = sizeof(pszFriendlyName);
        DWORD dwType = 0;
        // if (lpfnSETUPDIGETDEVICEREGISTRYPROPERTY(hDevInfoSet, &devInfo, SPDRP_DEVICEDESC, 
        //     &dwType, reinterpret_cast<PBYTE>(pszFriendlyName), dwSize, &dwSize) && (dwType == REG_SZ))
        //   sFriendlyNames.Add(pszFriendlyName);
        // else
        //   sFriendlyNames.Add(_T(""));
        if (SetupDiGetDeviceRegistryProperty(hDevInfoSet, &devInfo, SPDRP_FRIENDLYNAME, &dwType, 
               (PBYTE) pszFriendlyName, dwSize, &dwSize) && (dwType == REG_SZ))
         {
            // printf("found port %u, %s\n", nPort, pszFriendlyName) ;
            if (!match_port_number(pszFriendlyName, nPort)) 
               add_new_port(nPort, pszFriendlyName) ;
         }  
         else {
           // sFriendlyNames.Add(_T(""));
           // strncpy(si->strFriendlyName, "no name", MAX_PATH_LEN) ;
            printf("did NOT found port %u\n", nPort) ;
         }

      }
    }

    ++nIndex;
  }

  //Free up the "device information set" now that we are finished with it
  SetupDiDestroyDeviceInfoList(hDevInfoSet);

  //Unload the setup dll
//   FreeLibrary(hSetupAPI);

  //Return the success indicator
  return TRUE;
}

//****************************************************************************
//  Enumerate serial ports on Windows XP
//  from: http://www.naughter.com/enumser.html  
//****************************************************************************
//  This was originally enumerating all of the serial ports,
//  but it was not providing the path for opening them!!
//  Now, I'm going to use the SPDRP_DEVICEDESC from each method
//  to merge the two together and add the COMn port number to our data.
//****************************************************************************
static BOOL UsingSetupAPI1(void)
{
   // char errstr[128] ;
   //Make sure we clear out any elements which may already be in the array(s)
   // ports.RemoveAll();
   // sFriendlyNames.RemoveAll();

   //Now create a "device information set" which is required to enumerate all the ports
   GUID guid = GUID_DEVINTERFACE_COMPORT;
   HDEVINFO hDevInfoSet = SetupDiGetClassDevs(&guid, NULL, NULL, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);
   if (hDevInfoSet == INVALID_HANDLE_VALUE) {
      syslog("SetupDiGetClassDevs: %s\n", get_system_message()) ;
      return FALSE;
   }

   //Finally do the enumeration
   BOOL bMoreItems = TRUE;
   int nIndex = 0;
   SP_DEVINFO_DATA devInfo;
   while (bMoreItems) {
     //Enumerate the current device
     devInfo.cbSize = sizeof(SP_DEVINFO_DATA);
     bMoreItems = SetupDiEnumDeviceInfo(hDevInfoSet, nIndex, &devInfo);
     if (bMoreItems) {
        //************************************************************************
        //  initially, this process will not attempt to be rigorously clean;
        //  i.e., if any of the reads fail, it will not clean up the temp data.
        //  Later, in production code, we'll clean this up.
        //************************************************************************
        // SSerInfo_p si = new SSerInfo_t ;
        // memset((char *) si, 0, sizeof(SSerInfo_t)) ;

       //Did we find a serial port for this device
       BOOL bAdded = FALSE;
       unsigned nPort = 0 ;

       //Get the registry key which stores the ports settings
       HKEY hDeviceKey = SetupDiOpenDevRegKey(hDevInfoSet, &devInfo, DICS_FLAG_GLOBAL, 0, DIREG_DEV, KEY_QUERY_VALUE);
       if (hDeviceKey) {
         //Read in the name of the port
         TCHAR pszPortName[256];
         DWORD dwSize = sizeof(pszPortName);
         DWORD dwType = 0;
         if ((RegQueryValueEx(hDeviceKey, _T("PortName"), NULL, &dwType, 
               reinterpret_cast<LPBYTE>(pszPortName), &dwSize) == ERROR_SUCCESS) && 
               (dwType == REG_SZ)) {
           //If it looks like "COMX" then
           //add it to the array which will be returned
           size_t nLen = lstrlen(pszPortName);
           if (nLen > 3) {
             if ((_tcsnicmp(pszPortName, _T("COM"), 3) == 0) && 
                  pszPortName[3] >= '0'  &&
                  pszPortName[3] <= '9')
                  // IsNumeric(&pszPortName[3], FALSE))
             {
               //Work out the port number
               nPort = _ttoi(&pszPortName[3]);
               // nPort = atoi(&pszPortName[3]);
               // printf("found port %u\n", nPort) ;
               // ports.Add(nPort);
               // si->portnum = nPort ;

               bAdded = TRUE;
             }
           }
         }

         //Close the key now that we are finished with it
         RegCloseKey(hDeviceKey);
       }

       //If the port was a serial port, then also try to get its friendly name
       if (bAdded) {
         TCHAR pszFriendlyName[256];
         DWORD dwSize = sizeof(pszFriendlyName);
         DWORD dwType = 0;
         if (SetupDiGetDeviceRegistryProperty(hDevInfoSet, &devInfo, SPDRP_FRIENDLYNAME, &dwType, (PBYTE) pszFriendlyName, dwSize, &dwSize) && (dwType == REG_SZ))
           // sFriendlyNames.Add(pszFriendlyName);
           // strncpy(si->strFriendlyName, pszFriendlyName, MAX_PATH_LEN) ;
         {
            // printf("\nfound port %u, %s\n", nPort, pszFriendlyName) ;
            match_port_number(pszFriendlyName, nPort) ;
         }  
         else {
           // sFriendlyNames.Add(_T(""));
           // strncpy(si->strFriendlyName, "no name", MAX_PATH_LEN) ;
         }
       }
     }   //  if device info acquired
     else {
         // wsprintf(errstr, "index %d: SetupDiEnumDeviceInfo: %s\n", nIndex, get_system_message()) ;
         // OutputDebugString(errstr) ;
     }

     ++nIndex;
   }

   //Free up the "device information set" now that we are finished with it
   SetupDiDestroyDeviceInfoList(hDevInfoSet);

   //Return the success indicator
   return TRUE;
}

//****************************************************************************
//  Dig port data out of the registry
//****************************************************************************
static BOOL EnumPortsWdm(void)
{
   BOOL bOk ;
   SP_DEVICE_INTERFACE_DATA ifcData;
   DWORD dwDetDataSize ;
   // Create a device information set that will be the container for 
   // the device interfaces.
   GUID *guidDev = (GUID*) &GUID_CLASS_COMPORT;

   SP_DEVICE_INTERFACE_DETAIL_DATA *pDetData = NULL;

   HDEVINFO hDevInfo = SetupDiGetClassDevs( guidDev,
      NULL,
      NULL,
      DIGCF_PRESENT | DIGCF_DEVICEINTERFACE
      );

   if(hDevInfo == INVALID_HANDLE_VALUE) {
      syslog("SetupDiGetClassDevs: %s\n", get_system_message()) ;
      goto error_exit;
   }

   // Enumerate the serial ports
   bOk = TRUE;
   dwDetDataSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA) + 256;
   pDetData = (SP_DEVICE_INTERFACE_DETAIL_DATA*) new char[dwDetDataSize];
   // This is required, according to the documentation.  Yes, it's weird.
   ifcData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);
   pDetData->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);
   for (DWORD ii=0; bOk; ii++) {
      bOk = SetupDiEnumDeviceInterfaces(hDevInfo,
         NULL, guidDev, ii, &ifcData);
      if (bOk) {
         // Got a device. Get the details.
         SP_DEVINFO_DATA devdata = {sizeof(SP_DEVINFO_DATA)};  //lint !e785
         bOk = SetupDiGetDeviceInterfaceDetail(hDevInfo,
            &ifcData, pDetData, dwDetDataSize, NULL, &devdata);
         if (bOk) {
            //  allocate initial struct
            SSerInfo_p si = new SSerInfo_t ;
            memset((char *) si, 0, sizeof(SSerInfo_t)) ;

            // strncpy(si->strDevPath, pDetData->DevicePath, MAX_PATH_LEN);
            lstrcpyn(si->strDevPath, pDetData->DevicePath, MAX_PATH_LEN);
            // Got a path to the device. Try to get some more info.
            BOOL bSuccess = SetupDiGetDeviceRegistryProperty(hDevInfo, &devdata, 
               SPDRP_FRIENDLYNAME, NULL, (PBYTE) si->strFriendlyName, MAX_PATH_LEN, NULL);
            si->bEnumerates = true ;

            //  SPDRP_DEVICEDESC is subset of SPDRP_FRIENDLYNAME
            // bSuccess = bSuccess && SetupDiGetDeviceRegistryProperty(
            //    hDevInfo, &devdata, SPDRP_DEVICEDESC, NULL,
            //    (PBYTE)desc, sizeof(desc), NULL);

            // BOOL bUsbDevice = FALSE;
            // TCHAR locinfo[256];
            // if (SetupDiGetDeviceRegistryProperty(
            //    hDevInfo, &devdata, SPDRP_LOCATION_INFORMATION, NULL,
            //    (PBYTE)locinfo, sizeof(locinfo), NULL))
            // {
            //    //  Just check the first three characters to determine
            //    //  if the port is connected to the USB bus. This isn't
            //    //  an infallible method; it would be better to use the BUS GUID. 
            //    //  Currently, Windows doesn't let you query that though
            //    //  (SPDRP_BUSTYPEGUID seems to exist in documentation only).
            //    //  
            //    //  DDM note:  and, in fact, this fails to recognize that my
            //    //  Olimex USB JTAG device is a USB device.
            //    bUsbDevice = (strncmp(locinfo, "USB", 3)==0);
            // }
            if (bSuccess) {
               si->index = port_count++ ;
               
               //  add new struct to list
               if (sp_top == 0) 
                  sp_top = si ;
               else 
                  sp_tail->next = si ;
               sp_tail = si ;
            }

         }
         else {
            // strErr.Format("SetupDiGetDeviceInterfaceDetail failed. (err=%lx)",
            //    GetLastError());
            // throw strErr;
            syslog("SetupDiGetDeviceInterfaceDetail: %s\n", get_system_message()) ;
            goto error_exit;
         }
      }
      else {
         DWORD err = GetLastError();
         if (err != ERROR_NO_MORE_ITEMS) {
            // strErr.Format("SetupDiEnumDeviceInterfaces failed. (err=%lx)", err);
            // throw strErr;
            syslog("SetupDiEnumDeviceInterfaces: %s\n", get_system_message()) ;
            goto error_exit;
         }
      }
   }
   if (pDetData != NULL)   //lint !e774
      delete [] (char*)pDetData;
   if (hDevInfo != INVALID_HANDLE_VALUE)
      SetupDiDestroyDeviceInfoList(hDevInfo);

   // if (!strErr.IsEmpty())
   //    throw strErr;
   return TRUE ;
error_exit:
   if (pDetData != NULL)   //lint !e774
      delete [] (char*)pDetData;
   if (hDevInfo != INVALID_HANDLE_VALUE)
      SetupDiDestroyDeviceInfoList(hDevInfo);
   return FALSE;
}

//****************************************************************************
// Routine for enumerating the available serial ports.
// Throws a CString on failure, describing the error that
// occurred. If bIgnoreBusyPorts is TRUE, ports that can't
// be opened for read/write access are not included.
//****************************************************************************
// void EnumSerialPorts(SSerInfo_p asi)
void EnumSerialPorts(void)
{
   // Clear the output array
   // asi.RemoveAll();

   // Use different techniques to enumerate the available serial
   // ports, depending on the OS we're using
   OSVERSIONINFO vi;
   vi.dwOSVersionInfoSize = sizeof(vi);
   if (!GetVersionEx(&vi)) {
      syslog("Could not get OS version. (err=%lx)\n", GetLastError());
      goto build_compatibility_mode;
   }
   // Handle windows 9x and NT4 specially
   if (vi.dwMajorVersion < 5) {
      if (vi.dwPlatformId == VER_PLATFORM_WIN32_NT)
         syslog("This function does not support WinNT4\n") ;
      else
         syslog("This function does not support Win9x\n") ;
      goto build_compatibility_mode;
   }
   // Win2k and later support a standard API for
   // enumerating hardware devices.
   // if (!EnumPortsWdm(asi))
   // goto build_compatibility_mode;
   EnumPortsWdm() ;
   // if (!EnumPortsWdm())
   //    goto build_compatibility_mode;

   //  find COMM port number for each entry
   // UsingSetupAPI1();
   // unsigned ports[256] ;
   // UsingSetupAPI2(&ports[0]);
   UsingSetupAPI2();

   //  01/01/15 - move this test until after *both* of the previous functions
   //  have been run.  They search different parts of the registry.
   if (sp_top == 0)
      goto build_compatibility_mode;

   //  look for other ports which do not enumerate
   // CountCommPorts() ;

   //  test each port to see if it will enumerate
   SSerInfo_p rsi ;
   for (rsi = sp_top; rsi != 0; rsi = rsi->next) {
       // Only display ports that can be opened for read/write
       HANDLE hCom = CreateFile(rsi->strDevPath,
          GENERIC_READ | GENERIC_WRITE,
          0,    /* comm devices must be opened w/exclusive-access */
          NULL, /* no security attrs */
          OPEN_EXISTING, /* comm devices must use OPEN_EXISTING */
          0,    /* not overlapped I/O */
          NULL  /* hTemplate must be NULL for comm devices */
          );
       if (hCom == INVALID_HANDLE_VALUE) {
          // It can't be opened; remove it.
          //  asi.RemoveAt(ii);   //  this is *one* thing that's not easy here
          // ii--;               
          // printf("cannot open %s\n", rsi->strDevPath) ;
          // continue;
       } 
       else {
          rsi->bDeviceOpens = TRUE ;
          // It can be opened! Close it and add it to the list
          CloseHandle(hCom);
       }
   }

   return ;

build_compatibility_mode:
   syslog("no serial ports found...\n");
   //  compatibility backup
   // if (sp_top == 0) {
   //    syslog("building default serial-port list\n") ;
   //    unsigned j ;
   //    for (j=1; j<=4; j++) {
   //       add_new_port(j) ;
   //    }
   // }
}

//****************************************************************************
#define  MAX_UNICODE_LEN   1024
char *unicode2ascii(WCHAR *UnicodeStr)
{
   static char AsciiStr[MAX_UNICODE_LEN+1] ;
   WideCharToMultiByte(CP_ACP, 0, UnicodeStr, -1, AsciiStr, MAX_UNICODE_LEN, NULL, NULL);
   return AsciiStr ;
}

//****************************************************************************
static void usage(void)
{
   puts("Usage: serial_enum [-d]");
   puts("Option -d displays device name, as well as other data.");
}

//****************************************************************************
#ifdef  STAND_ALONE
int main(int argc, char **argv)
{
   bool show_device_name = false ; 

   //  parse command line
   int idx ;
   for (idx=1; idx<argc; idx++) {
      char *p = argv[idx];
      if (*p == '-'  ||  *p == '/') {
         p++ ;
         switch (*p) {
         case 'd':
            show_device_name = true ;
            break ;
         default :
            usage() ;
            return 1 ;   
         }
      } else {
         usage() ;
         return 1 ;
      }
   }

   // Populate the list of serial ports.
   EnumSerialPorts();

   // printf("found %u ports\n", port_count) ;
   SSerInfo_p sptemp ;
   printf("Port   Opens  Enums  Device description\n") ;
   printf("=====  =====  =====  ============================\n") ;
   for (sptemp = sp_top; sptemp != 0; sptemp = sptemp->next) {
      printf("COM%-2u   %-3s    %-3s   %s\n", 
         sptemp->portnum,
         (sptemp->bDeviceOpens) ? "YES" : "no",
         (sptemp->bEnumerates) ? "YES" : "no",
         unicode2ascii(sptemp->strFriendlyName)
         ) ;
      if (show_device_name)
         printf("   %s\n", unicode2ascii(sptemp->strDevPath)) ;
   }
   return 0;
}
#endif 
