/* Copyright(c) 2017, One Unified. All rights reserved.                 *
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
 * File:   InstrumentActions.h
 * Author: rpb
 *
 * Created on February 25, 2017, 7:57 PM
 */

#pragma once

#include <boost/shared_ptr.hpp>
#include <boost/signals2.hpp>

#include <wx/treebase.h>

#include <TFBitsNPieces/FirstOrDefaultCombiner.h>

// todo:  remove the references to pWatch_t.  a different class needs to handle the data, this structure is for messaging
// which suggests this should be sent to worker queues, or futures or packages
// which work in the background, and ultimately generate gui events

class InstrumentActions {
public:
  
  // instrument dialog locks to option or futuresoption when adding sub-menu to underlying instrument
  enum ENewInstrumentLock { NoLock, LockOption, LockFuturesOption };
  
  typedef boost::shared_ptr<InstrumentActions> pInstrumentActions_t;
  
  struct values_t {
    std::string name_;
    ENewInstrumentLock lockType_;
    values_t( const std::string& name, const ENewInstrumentLock lockType )
      : name_( name ), lockType_( lockType ) {}
    values_t( const values_t& rhs ): name_( rhs.name_ ), lockType_( rhs.lockType_ ) {}
    values_t( void ): lockType_( ENewInstrumentLock::NoLock ) {}
  };
  
  // used in TreeItemInstrument
  typedef boost::signals2::signal<values_t (const wxTreeItemId&, ENewInstrumentLock), 
                                  ou::tf::FirstOrDefault<values_t> > signalNewInstrument_t;
  typedef signalNewInstrument_t::slot_type slotNewInstrument_t;
  signalNewInstrument_t signalNewInstrument;
  
  typedef boost::signals2::signal<void (const wxTreeItemId&, const std::string&)> signalLoadInstrument_t;
  typedef signalLoadInstrument_t::slot_type slotLoadInstrument_t;
  signalLoadInstrument_t signalLoadInstrument;
  
  typedef boost::signals2::signal<ENewInstrumentLock (const wxTreeItemId&),
                                  ou::tf::FirstOrDefault<ENewInstrumentLock> > signalGetLockType_t;
  typedef signalGetLockType_t::slot_type slotGetLockType_t;
  signalGetLockType_t signalGetLockType;
  
  typedef boost::signals2::signal<void (const wxTreeItemId&)> signalLiveChart_t;
  typedef signalLiveChart_t::slot_type slotLiveChart_t;
  signalLiveChart_t signalLiveChart;
  
  typedef boost::signals2::signal<void (const wxTreeItemId&)> signalDailyChart_t;
  typedef signalDailyChart_t::slot_type slotDailyChart_t;
  signalDailyChart_t signalDailyChart;
  
  typedef boost::signals2::signal<void (const wxTreeItemId&)> signalEmitValues_t;
  typedef signalEmitValues_t::slot_type slotEmitValues_t;
  signalEmitValues_t signalEmitValues;
  
  typedef boost::signals2::signal<void (const wxTreeItemId&)> signalSaveData_t;
  typedef signalSaveData_t::slot_type slotSaveData_t;
  signalSaveData_t signalSaveData;
  
  typedef boost::signals2::signal<void (const wxTreeItemId&)> signalDelete_t;
  typedef signalDelete_t::slot_type slotDelete_t;
  signalDelete_t signalDelete;
  
  InstrumentActions();
  virtual ~InstrumentActions();

protected:
  
private:

};
