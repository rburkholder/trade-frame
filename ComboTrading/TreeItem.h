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

#include <wx/panel.h>

#include <TFTrading/Instrument.h>

#include <TFBitsNPieces/TreeOpsItems.h>

struct Resources {
  // will need signal for obtaining iqfeed symbol list for use in validation
  //wxPanel* m_pPanel;  // re-usable for charts and stuff
  wxWindow* m_pWin;
  
  enum InstrumentAction {
    IANewInstrument
  };
  typedef ou::tf::Instrument::pInstrument_t pInstrument_t;
  typedef boost::signals2::signal<pInstrument_t (void), ou::tf::FirstOrDefault<pInstrument_t> > signalNewInstrument_t;
  typedef signalNewInstrument_t::slot_type slotNewInstrument_t;
  signalNewInstrument_t signalNewInstrument;
  
  Resources( void ): m_pWin( 0 ) {}
};

// ================

class TreeItemResources: public ou::tf::TreeItemBase {
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
