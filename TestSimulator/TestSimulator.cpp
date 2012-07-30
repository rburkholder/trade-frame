/************************************************************************
 * Copyright(c) 2012, One Unified. All rights reserved.                 *
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

// TestSimulator.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "CreateDataSet.h"
#include "RunSequence.h"

int _tmain(int argc, _TCHAR* argv[]) {

//  CreateDataSet cds;
//  cds.Save();

  RunSequence seq(date( 2012, 7, 22 ));
  seq.Run();

	return 0;
}

