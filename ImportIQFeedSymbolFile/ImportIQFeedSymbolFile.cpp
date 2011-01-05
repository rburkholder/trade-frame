/************************************************************************
 * Copyright(c) 2009, One Unified. All rights reserved.                 *
 *                                                                      *
 * This file is provided as is WITHOUT ANY WARRANTY                     *
 *  without even the implied warranty of                                *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.                *
 *                                                                      *
 * This software may not be used nor distributed without proper license *
 * agreement.                                                           *
 *                                                                      *
 * See the file LICENSE.txt for redistribution information.             *
 ************************************************************************/

// ImportIQFeedSymbolFile.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <LibIQFeed\IQFeedSymbolFile.h>


int _tmain(int argc, _TCHAR* argv[]) {

  CIQFeedSymbolFile file;

  file.Load( "mktsymbols_v2.txt", "symbols.db4" );

	return 0;
}

