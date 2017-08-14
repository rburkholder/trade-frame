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
#include <wx/string.h>

#include <TFBitsNPieces/FirstOrDefaultCombiner.h>

// todo:
//    this structure is for messaging
// which suggests this should be sent to worker queues, or futures or packages
// which work in the background, and ultimately generate gui events

class InstrumentActions {
public:
  
  // * instrument dialog may be limited to a subset of instruments
  // * menu tree presents certain options depending upon what is allowed
  // * need to get this out of here for more generic use, maybe in the enumerations file
  enum EAllowedInstrumentSelectors { AllAllowed, OptionsAllowed, FuturesOptionsAllowed, NoneAllowed };
  
  typedef boost::shared_ptr<InstrumentActions> pInstrumentActions_t;
  
  struct values_t {
    std::string name_;
    EAllowedInstrumentSelectors selector;
    values_t( void ): selector( EAllowedInstrumentSelectors::AllAllowed ) {}
    values_t( const values_t& rhs ): name_( rhs.name_ ), selector( rhs.selector ) {}
    values_t( const std::string& name, const EAllowedInstrumentSelectors selector_ )
      : name_( name ), selector( selector_ ) {}
  };
  
  // used in TreeItemInstrument
  typedef boost::signals2::signal<values_t (const wxTreeItemId&, EAllowedInstrumentSelectors, const wxString&), 
                                  ou::tf::FirstOrDefault<values_t> > signalNewInstrument_t;
  typedef signalNewInstrument_t::slot_type slotNewInstrument_t;
  signalNewInstrument_t signalNewInstrument;
  
  typedef boost::signals2::signal<void (const wxTreeItemId&, const std::string&, const std::string&)> signalLoadInstrument_t;
  typedef signalLoadInstrument_t::slot_type slotLoadInstrument_t;
  signalLoadInstrument_t signalLoadInstrument;
  
  typedef boost::signals2::signal<void (const wxTreeItemId&)> signalLiveChart_t;
  typedef signalLiveChart_t::slot_type slotLiveChart_t;
  signalLiveChart_t signalLiveChart;
  
  typedef boost::signals2::signal<void (const wxTreeItemId&)> signalDailyChart_t;
  typedef signalDailyChart_t::slot_type slotDailyChart_t;
  signalDailyChart_t signalDailyChart;
  
  typedef boost::signals2::signal<void (const wxTreeItemId&)> signalOptionList_t;
  typedef signalOptionList_t::slot_type slotOptionList_t;
  signalOptionList_t signalOptionList;
  
  typedef boost::signals2::signal<void (const wxTreeItemId&)> signalEmitValues_t;
  typedef signalEmitValues_t::slot_type slotEmitValues_t;
  signalEmitValues_t signalEmitValues;
  
//  typedef boost::signals2::signal<void (const wxTreeItemId&)> signalSaveData_t;
//  typedef signalSaveData_t::slot_type slotSaveData_t;
//  signalSaveData_t signalSaveData;
  
  typedef boost::signals2::signal<void (const wxTreeItemId&)> signalDelete_t;
  typedef signalDelete_t::slot_type slotDelete_t;
  signalDelete_t signalDelete;
  
  InstrumentActions();
  virtual ~InstrumentActions();

protected:
  
private:

};
