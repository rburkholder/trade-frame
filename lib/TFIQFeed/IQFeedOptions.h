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

#include <vector>
#include <string>

#include "LibCommon/FastDelegate.h"
using namespace fastdelegate;

#include "IQFeedRetrieval.h"

class CIQFeedOptions :
  public CIQFeedRetrieval {
public:

  CIQFeedOptions(CIQFeedProvider *pProvider, const char *);
  virtual ~CIQFeedOptions(void);

  typedef FastDelegate0<void> OnSymbolListReceivedHandler;
  void SetOnSymbolListRecieved( OnSymbolListReceivedHandler function ) {
    OnSymbolListReceived = function;
  }

  std::vector<std::string *> m_vOptionSymbols;

protected:
  virtual void OnNewResponse( const char *szLine );
  void AddOptionSymbol( const char *s, unsigned short cnt );
private:
  OnSymbolListReceivedHandler OnSymbolListReceived;
};
