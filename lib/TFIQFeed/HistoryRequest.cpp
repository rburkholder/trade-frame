/************************************************************************
 * Copyright(c) 2021, One Unified. All rights reserved.                 *
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

/*
 * File:    HistoryRequest.cpp
 * Author:  raymond@burkholder.net
 * Project: lib/TFIQFeed
 * Created on 2021/09/06 21:09
 */

#include "DailyHistory.h"
#include "HistoryRequest.h"

namespace ou {
namespace tf {
namespace iqfeed {

HistoryRequest::HistoryRequest(
  fConnected_t&& fConnected
)
: m_bInProcess( false ),
  m_fConnected( std::move( fConnected ) )
{
  m_pHistory = std::make_unique<DailyHistory>(
    [this](){ // fConnected_t
      m_fConnected();
    },
    [this]( const ou::tf::Bar& bar ){ // fBar_t
      m_entryCurrent.fBar( bar );
    },
    [this](){ // fDone_t
      m_entryCurrent.fDone();

      std::scoped_lock lock( m_mutexHistorySlots );
      if ( 0 != m_vEntry.size() ) {
        NextRequest( std::move( m_vEntry.back() ) );
        m_vEntry.pop_back();
      }
      else {
        m_bInProcess = false;
        m_entryCurrent.Clear();
      }
    }
  );
  m_pHistory->Connect(); // start the process
}

HistoryRequest::~HistoryRequest() {
  m_pHistory->Disconnect();
}

void HistoryRequest::Request( const std::string& sSymbol_, uint16_t nBar, fBar_t&& fBar, fDone_t&& fDone ) {
  const std::string sSymbol( sSymbol_ );
  Entry entry( std::move( sSymbol ), nBar, std::move( fBar ), std::move( fDone ) );
  std::scoped_lock lock( m_mutexHistorySlots );
  if ( m_bInProcess ) {
    m_vEntry.emplace_back( std::move( entry ) );
  }
  else {
    m_bInProcess = true;
    NextRequest( std::move( entry ) );
  }
}

void HistoryRequest::NextRequest( Entry&& entry ) {
  m_entryCurrent = std::move( entry );
  m_pHistory->Request( m_entryCurrent.sSymbol, m_entryCurrent.nBar );
}

} // namespace iqfeed
} // namespace tf
} // namespace ou
