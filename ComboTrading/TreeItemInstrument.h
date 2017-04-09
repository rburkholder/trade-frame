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

#include "InstrumentActions.h"
#include "TreeItem.h"

class TreeItemInstrument: public TreeItemResources {
  friend class boost::serialization::access;
public:
  
  TreeItemInstrument( wxTreeItemId id, ou::tf::TreeItemResources& baseResources, Resources& resources );
  virtual ~TreeItemInstrument( void );
  
  virtual void ShowContextMenu( void );
  
  void HandleMenuNewInstrument( wxCommandEvent& event );
  
  // todo: invocable only if no instrument already exists
  bool NewInstrumentViaDialog( InstrumentActions::ENewInstrumentLock lock ); 
  
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

  void HandleMenuAddFuturesOption( wxCommandEvent& event );
  void HandleMenuAddOption( wxCommandEvent& event );
  void HandleLiveChart( wxCommandEvent& event );
  void HandleDailyChart( wxCommandEvent& event );
  void HandleSaveData( wxCommandEvent& event );
  void HandleDelete( wxCommandEvent& event );
  void HandleEmit( wxCommandEvent& event );
  
private:
  
  typedef InstrumentActions::pInstrumentActions_t pInstrumentActions_t;
  
  InstrumentActions::ENewInstrumentLock m_lockType;
  
  pInstrumentActions_t m_pInstrumentActions;
    
  void InstrumentViaDialog( InstrumentActions::ENewInstrumentLock lock, const std::string& sPrompt );
  
  // need to keep track of (load, save)
  //   * menu item text (as it can be changed)
  //   * instrument code(s)

  template<typename Archive>
  void save( Archive& ar, const unsigned int version ) const {
    ar & boost::serialization::base_object<const TreeItemResources>(*this);
    
    ar & m_lockType;
    
    const mapMembers_t::size_type n = m_mapMembers.size();
    ar << n;
    for ( mapMembers_t::const_iterator iter = m_mapMembers.begin(); iter != m_mapMembers.end(); ++iter ) {
      ar << ( iter->second.m_type );
      switch ( iter->second.m_type ) {
	case IdInstrument:
	{
	  const TreeItemInstrument* p = dynamic_cast<TreeItemInstrument*>( iter->second.m_pTreeItemBase.get() );
          ar & *p;
	}
	break;
      }
    }
  }

  template<typename Archive>
  void load( Archive& ar, const unsigned int version ) {
    ar & boost::serialization::base_object<TreeItemResources>(*this);
    ar & m_lockType;
    // this is going to cause problems if renamed, so prevent a rename, ... is rename even available?
    m_pInstrumentActions->signalLoadInstrument( this->m_id, m_baseResources.signalGetItemText( m_id ) );
    // call InstrumentActions::Startup here?
    
    mapMembers_t::size_type n;
    ar & n;
    for ( mapMembers_t::size_type ix = 0; ix < n; ++ix ) {
      unsigned int type;
      ar & type;
      switch ( type ) {
	case IdInstrument:
	{
          TreeItemInstrument* p = AddTreeItem<TreeItemInstrument,IdTreeItemType>( "Instrument", IdInstrument, m_resources );
          ar & *p;
	}
	break;
      }
    }
    
  }

  BOOST_SERIALIZATION_SPLIT_MEMBER()

};
