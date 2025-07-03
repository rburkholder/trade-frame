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

// 2012/08/12 could try using std:priority_queue instead or boost::max_heap
#include <OUCommon/MinHeap.h>

#include <OUCommon/FastDelegate.h>
using namespace fastdelegate;

#include <TFTimeSeries/TimeSeries.h>

#include "MergeDatedDatumCarrier.hpp"

namespace ou { // One Unified
namespace tf { // TradeFrame

class MergeDatedDatums {
public:

  enum enumMergingState { eInit, eRunning, ePaused, eStopped };

  MergeDatedDatums();
  virtual ~MergeDatedDatums();

  typedef FastDelegate1<const DatedDatum &> OnDatumHandler;

  void Add( TimeSeries<Quote>& series, OnDatumHandler );
  void Add( TimeSeries<Trade>& series, OnDatumHandler );
  void Add( TimeSeries<Bar>& series, OnDatumHandler );
  void Add( TimeSeries<Greek>& series, OnDatumHandler );
  void Add( TimeSeries<DepthByMM>& series, OnDatumHandler );
  void Add( TimeSeries<DepthByOrder>& series, OnDatumHandler );
  void Run();
  void Stop();

  enumMergingState GetState() const { return m_state; };

  unsigned long GetCountProcessedDatums() const { return m_cntProcessedDatums; };

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
