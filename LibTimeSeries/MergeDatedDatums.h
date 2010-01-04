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

#include "libCommon/MinHeap.h"

#include "LIbCommon/FastDelegate.h"
using namespace fastdelegate;

#include "TimeSeries.h"
#include "MergeDatedDatumCarrier.h"

class CMergeDatedDatums {
public:

  CMergeDatedDatums(void);
  virtual ~CMergeDatedDatums(void);

  typedef FastDelegate1<const CDatedDatum &> OnDatumHandler;

  void Add( CTimeSeries<CQuote> *pSeries, OnDatumHandler );
  void Add( CTimeSeries<CTrade> *pSeries, OnDatumHandler );
  void Add( CTimeSeries<CBar> *pSeries, OnDatumHandler );
  void Add( CTimeSeries<CMarketDepth> *pSeries, OnDatumHandler );
  void Run( void );
  void Stop( void );

  unsigned long GetCountProcessedDatums( void ) { return m_cntProcessedDatums; };
  unsigned long GetCountCarrierReorders( void ) { return m_cntReorders; };

protected:

  CMinHeap<CMergeCarrierBase *, CMergeCarrierBase> m_vCarriers;

  // not all states or commands are implemented yet
  enum enumMergingState { eInit, eRunning, ePaused, eStopped };
  enum enumMergingCommands { eUnknown, eRun, eStop, ePause, eResume, eReset };

  enumMergingState m_state;
  enumMergingCommands m_request;

  unsigned long m_cntProcessedDatums;
  unsigned long m_cntReorders;

  inline virtual void ProcessCarrier( CMergeCarrierBase *pCarrier );

private:

};

