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
#include <TFTrading/Watch.h>

#include "TreeItem.h"

class TreeItemInstrument: public TreeItemResources {
  friend class boost::serialization::access;
public:
  typedef ou::tf::Instrument::pInstrument_t pInstrument_t;
  
  TreeItemInstrument( wxTreeItemId id, ou::tf::TreeItemResources& baseResources, Resources& resources );
  virtual ~TreeItemInstrument( void );
  
    void HandleMenuNewInstrument( wxCommandEvent& event );
  
  pInstrument_t GetInstrument( void ) { return m_pInstrument; }
  
  virtual void ShowContextMenu( void );
  
protected:

  enum IdTreeItemType {
    IdGroup = 301, IdInstrument
  };
    
  enum {
    ID_Null = wxID_HIGHEST,
    MINewInstrument, MINewOption, MINewFuturesOption,
    MILiveChart, MIDailyChart, MISaveData, MIEmit,
    MIDelete
  };
  
    void BuildContextMenu( wxMenu* pMenu );
    
    void NewInstrumentViaDialog( Resources::ENewInstrumentLock lock ); // invocable only if no instrument already exists
    void InstrumentViaDialog( const std::string& sPrompt, Resources::ENewInstrumentLock lock );
    
    void HandleMenuAddFuturesOption( wxCommandEvent& event );
    void HandleMenuAddOption( wxCommandEvent& event );
    void HandleLiveChart( wxCommandEvent& event );
    void HandleDailyChart( wxCommandEvent& event );
    void HandleSaveData( wxCommandEvent& event );
    void HandleDelete( wxCommandEvent& event );
    void HandleEmit( wxCommandEvent& event );
  
private:
  
  pInstrument_t m_pInstrument;
  
  ou::tf::Watch* m_pWatch;
  
  void Watch( void );  // will want to set signals on provider so watch/unwatch as provider transitions connection states
  
  template<typename Archive>
  void save( Archive& ar, const unsigned int version ) const {
    ar & boost::serialization::base_object<const TreeItemResources>(*this);
  }

  template<typename Archive>
  void load( Archive& ar, const unsigned int version ) {
    ar & boost::serialization::base_object<TreeItemResources>(*this);
    try {
      std::string s( m_baseResources.signalGetItemText( m_id ) );
      m_pInstrument = m_resources.signalLoadInstrument( s );
      Watch();
    }
    catch (std::runtime_error& e) {
      std::cout << "TreeItemInstrument: couldn't load instrument" << std::endl;
    }
  }

  BOOST_SERIALIZATION_SPLIT_MEMBER()

};
