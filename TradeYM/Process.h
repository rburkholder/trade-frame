/************************************************************************
 * Copyright(c) 2010, One Unified. All rights reserved.                 *
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

#include <LibInteractiveBrokers/IBTWS.h>
//#include <LibIQFeed/IQFeed.h>

class CProcess
//  : 
//  public CIQFeed<CProcess>
{
//  friend CIQFeed<CProcess>;
public:
  CProcess(void);
  ~CProcess(void);
protected:
  void OnIQFeedConnected( void );
  void OnIQFeedDisConnected( void );
//  void OnIQFeedUpdateMessage( linebuffer_t* pBuffer, CIQFUpdateMessage* msg);
private:
  CIBTWS m_tws;
};
