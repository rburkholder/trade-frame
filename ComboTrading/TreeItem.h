/************************************************************************
 * Copyright(c) 2015, One Unified. All rights reserved.                 *
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

// started December 13, 2015, 2:13 PM

#pragma once

#include <boost/signals2.hpp>

//#include <wx/panel.h>
//#include <wx/window.h>

#include <TFBitsNPieces/TreeOpsItems.h>
#include <TFTrading/Watch.h>

struct Resources {

  typedef ou::tf::Watch::pWatch_t pWatch_t;
  
  // instrument dialog locks to option or futuresoption when adding sub-menu to underlying instrument
  enum ENewInstrumentLock { NoLock, LockOption, LockFuturesOption };
  
  // used in TreeItemInstrument
  typedef boost::signals2::signal<pWatch_t (ENewInstrumentLock), ou::tf::FirstOrDefault<pWatch_t> > signalNewInstrumentWatch_t;
  typedef signalNewInstrumentWatch_t::slot_type slotNewInstrumentWatch_t;
  signalNewInstrumentWatch_t signalNewInstrumentViaDialog;
  
  typedef boost::signals2::signal<pWatch_t (const std::string&), ou::tf::FirstOrDefault<pWatch_t> > signalLoadInstrumentWatch_t;
  typedef signalLoadInstrumentWatch_t::slot_type slotLoadInstrumentWatch_t;
  signalLoadInstrumentWatch_t signalLoadInstrument;
  
};

// ================

class TreeItemResources: public ou::tf::TreeItemBase {
  friend class boost::serialization::access;
public:
  TreeItemResources( wxTreeItemId id, ou::tf::TreeItemResources& baseResources, Resources& resources ):
    ou::tf::TreeItemBase( id, baseResources ), m_resources( resources ) {}
  virtual ~TreeItemResources( void ) {};
protected:
  Resources& m_resources;
private:
  
  template<typename Archive>
  void save( Archive& ar, const unsigned int version ) const {
    ar & boost::serialization::base_object<const ou::tf::TreeItemBase>(*this);
  }

  template<typename Archive>
  void load( Archive& ar, const unsigned int version ) {
    ar & boost::serialization::base_object<ou::tf::TreeItemBase>(*this);
  }

  BOOST_SERIALIZATION_SPLIT_MEMBER()

};
