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

#include <TFTrading/TradingEnumerations.h>

#include "TreeItem.h"
#include "TreeItemInstrument.h"

class TreeItemGroup;

class TreeItemGroupCommon: public TreeItemResources {
  friend class boost::serialization::access;
public:

  TreeItemGroupCommon( wxTreeItemId id, ou::tf::TreeItemResources& baseResources, Resources& resources ):
    TreeItemResources( id, baseResources, resources ) {}
  virtual ~TreeItemGroupCommon( void ) {}

protected:

  enum IdTreeItemType {
    IdGroup = 201, IdInstrument, IdPortfolio, IdPosition
  };

  enum {
    ID_Null = wxID_HIGHEST,
    MIAddGroup, MIAddInstrument, MIAddPortfolio, MIAddPosition,
    MIRename, MIDelete
  };

  void BuildContextMenu( wxMenu* pMenu );

private:
  void HandleAddInstrument( wxCommandEvent& event );
  void HandleAddPortfolio( wxCommandEvent& event );
  void HandleAddPosition( wxCommandEvent& event );
  void HandleAddGroup( wxCommandEvent& event );
  void HandleRename( wxCommandEvent& event );

  template<typename Archive>
  void save( Archive& ar, const unsigned int version ) const {
    ar << boost::serialization::base_object<const TreeItemResources>(*this);
    const mapMembers_t::size_type n = m_mapMembers.size();
    ar << n;
    for ( mapMembers_t::const_iterator iter = m_mapMembers.begin(); iter != m_mapMembers.end(); ++iter ) {
      ar << ( iter->second.m_type );
      switch ( iter->second.m_type ) {
        case IdGroup:
        {
          const TreeItemGroup* p = dynamic_cast<TreeItemGroup*>( iter->second.m_pTreeItemBase.get() );
          ar & *p;
        }
        break;
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
    mapMembers_t::size_type n;
    ar & n;
    for ( mapMembers_t::size_type ix = 0; ix < n; ++ix ) {
      unsigned int type;
      ar & type;
      switch ( type ) {
        case IdGroup:
        {
          TreeItemGroup* p = AddTreeItem<TreeItemGroup,IdTreeItemType>( "Group", IdGroup, m_resources );
          ar & *p;
        }
        break;
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

// ================

class TreeItemRoot: public TreeItemGroupCommon {
  friend class boost::serialization::access;
public:
  TreeItemRoot( wxTreeItemId id, ou::tf::TreeItemResources& baseResources, Resources& resources ):
    TreeItemGroupCommon( id, baseResources, resources ) {}
  virtual ~TreeItemRoot( void ) {}

  virtual void ShowContextMenu( void );

protected:
private:
//  enum {
//    ID_Null = wxID_HIGHEST,
//    MIAddGroup
//  };

  template<typename Archive>
  void save( Archive& ar, const unsigned int version ) const {
    ar & boost::serialization::base_object<const TreeItemGroupCommon>(*this);
  }

  template<typename Archive>
  void load( Archive& ar, const unsigned int version ) {
    ar & boost::serialization::base_object<TreeItemGroupCommon>(*this);
  }

  BOOST_SERIALIZATION_SPLIT_MEMBER()

};

// ================

class TreeItemGroup: public TreeItemGroupCommon {
  friend class boost::serialization::access;
public:
  TreeItemGroup( wxTreeItemId id, ou::tf::TreeItemResources& baseResources, Resources& resources ):
    TreeItemGroupCommon( id, baseResources, resources ) {}
  virtual ~TreeItemGroup( void ) {}

  virtual void ShowContextMenu( void );

protected:
private:

  void HandleDelete( wxCommandEvent& event );

  template<typename Archive>
  void save( Archive& ar, const unsigned int version ) const {
    ar & boost::serialization::base_object<const TreeItemGroupCommon>(*this);
  }

  template<typename Archive>
  void load( Archive& ar, const unsigned int version ) {
    ar & boost::serialization::base_object<TreeItemGroupCommon>(*this);
  }

  BOOST_SERIALIZATION_SPLIT_MEMBER()

};
