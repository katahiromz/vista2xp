// targetver.h --- target versioning header for Win             -*- C++ -*-
//////////////////////////////////////////////////////////////////////////////

// targetver.h is used to define the Win32API macros that target the 
// version of the Win you wish to support.

// For Win95
//#define WINVER          0x0400
//#define _WIN32_WINDOWS  0x0400
//#define _WIN32_WINNT    0x0400
//#define _WIN32_IE       0x0300

// For Win98
//#define WINVER          0x0410
//#define _WIN32_WINDOWS  0x0410
//#define _WIN32_WINNT    0x0410
//#define _WIN32_IE       0x0401

// For WinNT4
//#define WINVER          0x0400
//#define _WIN32_WINNT    0x0400
//#define _WIN32_IE       0x0200
//#define NTDDI_VERSION   0x05000000

// For WinME
//#define WINVER          0x0500
//#define _WIN32_WINNT    0x0500
//#define _WIN32_IE       0x0500

// For Win2000
//#define WINVER          0x0500
//#define _WIN32_WINNT    0x0500
//#define _WIN32_IE       0x0500
//#define NTDDI_VERSION   0x05000000

// For WinXP
#define WINVER          0x0501
#define _WIN32_WINNT    0x0501
#define _WIN32_IE       0x0501
#define NTDDI_VERSION   0x05010000

// For WinVista
//#define WINVER          0x0600
//#define _WIN32_WINNT    0x0600
//#define _WIN32_IE       0x0600
//#define NTDDI_VERSION   0x06000000

// For Win7
//#define WINVER          0x0601
//#define _WIN32_WINNT    0x0601
//#define _WIN32_IE       0x0601
//#define NTDDI_VERSION   0x06010000

// For Win8.1
//#define WINVER          0x0602
//#define _WIN32_WINNT    0x0602
//#define _WIN32_IE       0x0602
//#define NTDDI_VERSION   0x06030000

// For Win10
//#define WINVER          0x0A00
//#define _WIN32_WINNT    0x0A00
//#define _WIN32_IE       0x0A00
//#define NTDDI_VERSION   0x0A000000

// Users of Visual Studio 10+ can do this instead
// #include "SDKDDKver.h"

//////////////////////////////////////////////////////////////////////////////
