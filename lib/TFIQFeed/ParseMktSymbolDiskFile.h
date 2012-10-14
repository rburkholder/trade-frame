/************************************************************************
 * Copyright(c) 2012, One Unified. All rights reserved.                 *
 * email: info@oneunified.net                                           *
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

#include <OUCommon/FastDelegate.h>
using namespace fastdelegate;

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace iqfeed { // IQFeed

class ParseMktSymbolDiskFile {
public:

  typedef const char* iterator_t;
  typedef FastDelegate2<iterator_t&,iterator_t&> OnProcessLine_t;

  void SetOnProcessLine( OnProcessLine_t function ) {
    m_OnProcessLine = function;
  }

  ParseMktSymbolDiskFile( void );
  ~ParseMktSymbolDiskFile( void ) {};

  void Run( void );

protected:
private:
  OnProcessLine_t m_OnProcessLine;
};

} // namespace iqfeed
} // namespace tf
} // namespace ou

