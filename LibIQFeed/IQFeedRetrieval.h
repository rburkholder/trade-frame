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

#include "IQFeed.h"
#include "IQFeedProvider.h"

class CIQFeedRetrieval {
public:
  CIQFeedRetrieval( CIQFeedProvider *pProvider );
  virtual ~CIQFeedRetrieval(void);
  void OpenPort( void );
  void ClosePort( void );

protected:
  bool m_bLookingForDetail;
  CIQFeedThreadCrossing *m_pPort;

  CIQFeedProvider *m_pIQFeedProvider;
  virtual void OnNewResponse( const char *szLine ) = 0;
private:
};
