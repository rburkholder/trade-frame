/************************************************************************
 * Copyright(c) 2009, One Unified. All rights reserved.                 *
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

#include <vector>

#include <OUCommon/MinHeap.h>

#include <OUCommon/FastDelegate.h>
using namespace fastdelegate;

#include "TimeSeries.h"
#include "MergeDatedDatumCarrier.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

class MergeDatedDatums {
public:

  enum enumMergingState { eInit, eRunning, ePaused, eStopped };

  MergeDatedDatums(void);
  virtual ~MergeDatedDatums(void);

  typedef FastDelegate1<const DatedDatum &> OnDatumHandler;

  void Add( TimeSeries<Quote>& series, OnDatumHandler );
  void Add( TimeSeries<Trade>& series, OnDatumHandler );
  void Add( TimeSeries<Bar>& series, OnDatumHandler );
  void Add( TimeSeries<Greek>& series, OnDatumHandler );
  void Add( TimeSeries<MarketDepth>& series, OnDatumHandler );
  void Run( void );
  void Stop( void );

  enumMergingState GetState( void ) const { return m_state; };

  unsigned long GetCountProcessedDatums( void ) const { return m_cntProcessedDatums; };

protected:

  ou::CMinHeap<MergeCarrierBase*, MergeCarrierBase> m_mhCarriers;

  // not all states or commands are implemented yet
  enum enumMergingCommands { eUnknown, eRun, eStop, ePause, eResume, eReset };

  enumMergingState m_state;
  enumMergingCommands m_request;

  unsigned long m_cntProcessedDatums;

private:

};

} // namespace tf
} // namespace ou
