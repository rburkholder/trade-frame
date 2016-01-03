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

// started December 13, 2015, 8:16 PM

#pragma once

#include <TFTrading/Instrument.h>

#include "TreeItem.h"

class TreeItemInstrument: public TreeItemResources {
  friend class boost::serialization::access;
public:
  typedef ou::tf::Instrument::pInstrument_t pInstrument_t;
  
  TreeItemInstrument( wxTreeItemId id, ou::tf::TreeItemResources& baseResources, Resources& resources ): 
    TreeItemResources( id, baseResources, resources ) {
      Init();
    }
  virtual ~TreeItemInstrument( void );
  
  pInstrument_t GetInstrument( void ) { return m_pInstrument; }
  
  virtual void ShowContextMenu( void );
  
protected:

  enum {
    ID_Null = wxID_HIGHEST,
    MILiveChart, MIDailyChart,
    MIDelete
  };
  
    void BuildContextMenu( wxMenu* pMenu );
    
    void HandleLiveChart( wxCommandEvent& event );
    void HandleDailyChart( wxCommandEvent& event );
    void HandleDelete( wxCommandEvent& event );
  
private:
  
  pInstrument_t m_pInstrument;
  
  void Init( void );
  
  template<typename Archive>
  void save( Archive& ar, const unsigned int version ) const {
    ar & boost::serialization::base_object<const TreeItemResources>(*this);
  }

  template<typename Archive>
  void load( Archive& ar, const unsigned int version ) {
    ar & boost::serialization::base_object<TreeItemResources>(*this);
  }

  BOOST_SERIALIZATION_SPLIT_MEMBER()

};
