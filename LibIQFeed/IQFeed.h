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

#pragma once

#include <codeproject/thread.h>  // class inbound messages

class CIQFeed: public CGuiThreadImpl<CIQFeed> {
public:

  CIQFeed(CAppModule* pModule);
  ~CIQFeed(void);


protected:
  BEGIN_MSG_MAP_EX(CIQFeed)
  END_MSG_MAP()

  BOOL InitializeThread( void );
  void CleanupThread( DWORD );

private:
};
