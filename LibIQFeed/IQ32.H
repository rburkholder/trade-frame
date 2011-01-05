//-----------------------------------------------------------
//-----------------------------------------------------------
//
//             System: SYS097
//       Program Name: IQ32.dll
//          PVCS Name: iqf_iq32
//        Module Name: IQ32.H
//
//-----------------------------------------------------------
//
//            Proprietary Software Product
//
//           Data Transmission Network Inc.
//           9110 West Dodge Road Suite 200
//               Omaha, Nebraska  68114
//
//    Copyright (c) by Data Transmission Network 2004
//                 All Rights Reserved
//
//
//-----------------------------------------------------------
// Module Description: Defines the entry point for the DLL application.
//         References: None
//           Compiler: Microsoft Visual C++ Version 6.0
//             Author: Natalie Hannan
//        Modified By: 
//
//-----------------------------------------------------------
//-----------------------------------------------------------
//
// REVISION HISTORY
//$Archive:   \\dtnpvcs\scm\eng\interquote\sys097\archives\iqf_iq32\IQ32.H-arc  $
//$Author:   Brian.Wood  $
//$Date:   Jun 18 2009 10:02:12  $
//$Log:   \\dtnpvcs\scm\eng\interquote\sys097\archives\iqf_iq32\IQ32.H-arc  $
//
//   Rev 1.7   Jun 18 2009 10:02:12   Brian.Wood
//Added the _T and define so that the file would work under unicode.
//
//   Rev 1.6   Mar 28 2007 17:10:30   Brian.Wood
//Added public enum for the iqFeedMessages.
//
//Added public code to register the IQFeedMessages message.
//
//   Rev 1.5   Apr 21 2006 10:56:46   natalie.hannan
//Fixed bug #2872 - issue where  RegisterClientApp() doesn't return if a connection with the servers cannot be established (Ex. Internet down. )
//
//   Rev 1.4   Apr 18 2006 13:01:34   natalie.hannan
//Fixed issue where  RegisterClientApp() doesn't return if a connection with the servers cannot be established (Ex. Internet down. )
//
//   Rev 1.3   15 Apr 2005 11:34:16   natalieh
//Changed the maximum number of clients to 20.
//
//   Rev 1.2   Oct 11 2004 13:26:42   natalieh
//Corrected file header.
//$Modtime:   May 26 2009 15:21:08  $
//$Revision:   1.7  $
//$Workfile:   IQ32.H  $
//
//-----------------------------------------------------------
//-----------------------------------------------------------
// STANDARD HEADERS
//-----------------------------------------------------------
#include <winsock.h>
#include <windows.h>
//-----------------------------------------------------------
// SYSTEM/APPLICATION HEADERS
//-----------------------------------------------------------
#ifndef IQ32_H
#define IQ32_H
//-----------------------------------------------------------

//////////////////////////////////////////////////////////////////////////////////////////
// The mouse event will receive the following values for the X and Y coordinates:
//
//		X  Y  Result
//		----------------------------
//		0  0  Normal startup
//		1  0  Startup Off-line
//		0  1  Login Failed
//		1  1  Exit progra
//
//////////////////////////////////////////////////////////////////////////////////////////
enum eIQFeedMessages { IQFeed_Active=0, IQFeed_Offline=1, IQFeed_Terminated=2, IQFeed_Login_Failed=200};

#ifdef _UNICODE 
static UINT m_IQFeedMessages = RegisterWindowMessage(_T("IQFeedMessages"));
#else
static UINT m_IQFeedMessages = RegisterWindowMessage("IQFeedMessages");
#endif

#pragma warning (disable:4236)

#define MAX_CLIENT		20
#define CLIENT_NORMAL	 0
#define CLIENT_OFFLINE	 1
#define CLIENT_QUIT		 2
#define CLIENT_LOGIN_FAILED	3

typedef void (__stdcall *CallbackType)(int, int);

extern CallbackType	CallbackFunction;

//--------------------------------------------------------------------------
//       Static Function Prototypes
//--------------------------------------------------------------------------

void __stdcall SetAutoLogin(const char* szUserName, const char* szPassword);
void __stdcall ClearAutoLogin();

void __stdcall SetCallbackFunction(CallbackType callback);
int  __stdcall RegisterClientApp(HWND hClient, char* szProductName, char* szProductKey, char* szProductVersion);
void __stdcall RemoveClientApp(HWND hClient);
void __stdcall RegisterSocketHandle(HWND connect, int client);
void __stdcall RemoveSocketHandle();
void __stdcall WaitForMessageMux();
void __stdcall ReleaseMessageMux();

short __stdcall CxSet();
short __stdcall CxGet();
short __stdcall ConnectToRobbins();
short __stdcall CloseRobbins();
short __stdcall SendTradeToRobbins(char *TType, char *Price, char *Action, char *Qty, char *Symbol, char *Month, char *Year);

#endif // IQ32_H
