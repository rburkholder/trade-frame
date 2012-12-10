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

#include <string>
#include <set>

#include <boost/thread/thread.hpp>
#include <boost/foreach.hpp>

#include <OUCommon/FastDelegate.h>
using namespace fastdelegate;

#include <TFTimeSeries/DatedDatum.h>
#include "SymbolSelection.h"

class Worker {
public:

  typedef FastDelegate0<> OnCompletionHandler;
  typedef SymbolSelection::setInstrumentInfo_t setInstrumentInfo_t;

  Worker( OnCompletionHandler f );
  ~Worker(void);

  template<typename Function>
  void IterateInstrumentList( Function f ) {
    BOOST_FOREACH( const SymbolSelection::InstrumentInfo& ii, m_setInstrumentInfo ) {
      f( ii.sName, ii.barLast );
    }
  }

  void Join( void ) { m_pThread->join(); };
  void operator()( void );

protected:
private:

  OnCompletionHandler m_OnCompletion;
  
  setInstrumentInfo_t m_setInstrumentInfo;

  boost::thread* m_pThread;
  
};

