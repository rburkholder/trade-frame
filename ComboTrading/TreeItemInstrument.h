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

#include <TFTrading/Watch.h>

#include "TreeItem.h"

class TreeItemInstrument: public TreeItemResources {
  friend class boost::serialization::access;
public:
  
  typedef ou::tf::Watch::pWatch_t pWatch_t;
  
  TreeItemInstrument( wxTreeItemId id, ou::tf::TreeItemResources& baseResources, Resources& resources );
  virtual ~TreeItemInstrument( void );
  
  void HandleMenuNewInstrument( wxCommandEvent& event );
  
  pWatch_t GetInstrumentWatch( void ) { return m_pInstrumentWatch; }
  
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
    void InstrumentViaDialog( Resources::ENewInstrumentLock lock, const std::string& sPrompt );
    
    void HandleMenuAddFuturesOption( wxCommandEvent& event );
    void HandleMenuAddOption( wxCommandEvent& event );
    void HandleLiveChart( wxCommandEvent& event );
    void HandleDailyChart( wxCommandEvent& event );
    void HandleSaveData( wxCommandEvent& event );
    void HandleDelete( wxCommandEvent& event );
    void HandleEmit( wxCommandEvent& event );
  
private:

  pWatch_t m_pInstrumentWatch;
  
  template<typename Archive>
  void save( Archive& ar, const unsigned int version ) const {
    ar & boost::serialization::base_object<const TreeItemResources>(*this);
  }

  template<typename Archive>
  void load( Archive& ar, const unsigned int version ) {
    ar & boost::serialization::base_object<TreeItemResources>(*this);
    try {
      std::string s( m_baseResources.signalGetItemText( m_id ) );
      m_pInstrumentWatch = m_resources.signalLoadInstrument( s );
      m_pInstrumentWatch->StartWatch();
    }
    catch (std::runtime_error& e) {
      std::cout << "TreeItemInstrument: couldn't load instrument" << std::endl;
    }
  }

  BOOST_SERIALIZATION_SPLIT_MEMBER()

};
