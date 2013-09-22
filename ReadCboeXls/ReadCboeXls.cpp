// ReadCboeXls.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <shellapi.h>
#include <crtdbg.h>

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>

#else // _WIN32

#define	FW_NORMAL	400
#define	FW_BOLD		700

#endif // _WIN32

#include <TFBitsNPieces/ReadCboeWeeklyOptions.h>









int _tmain(int argc, _TCHAR* argv[]) {

  ou::tf::cboe::vExpiries_t vExpiries;
  ou::tf::cboe::vUnderlyinginfo_t vui;

  try {
    ou::tf::cboe::ReadCboeWeeklyOptions( vExpiries, vui );
  }
  catch(...) {
  }

	return 0;
}

