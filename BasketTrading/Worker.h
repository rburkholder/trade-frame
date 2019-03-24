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

// Project: BasketTrading

#include <set>
#include <string>
#include <algorithm>

#include <boost/thread/thread.hpp>

#include <OUCommon/FastDelegate.h>
using namespace fastdelegate;

#include <TFTimeSeries/DatedDatum.h>
#include "SymbolSelection.h"

class Worker {
public:

  typedef FastDelegate0<> OnCompletionHandler;  // TODO: convert to std::function or lambda
  typedef SymbolSelection::setInstrumentInfo_t setInstrumentInfo_t;

  Worker( OnCompletionHandler f );
  ~Worker(void);

  template<typename Function>
  void IterateInstrumentList( Function f ) {
    std::for_each( m_setInstrumentInfo.begin(), m_setInstrumentInfo.end(),
                  [&f](const SymbolSelection::InstrumentInfo& ii){
                    f( ii.sName, ii.barLast, ii.dblStop );
                  } );
  }

  void Join( void ) { m_pThread->join(); };
  void operator()( void );

protected:
private:

  OnCompletionHandler m_OnCompletion;

  setInstrumentInfo_t m_setInstrumentInfo;

  boost::thread* m_pThread;

};

