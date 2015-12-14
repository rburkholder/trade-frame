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

#include "TreeItem.h"

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
    const vMembers_t::size_type n = m_vMembers.size();
    ar << n;
    for ( vMembers_t::const_iterator iter = m_vMembers.begin(); iter != m_vMembers.end(); ++iter ) {
      ar << ( iter->m_type );
      switch ( iter->m_type ) {
        case IdGroup:
        {
          const TreeItemGroup* pGroup = dynamic_cast<TreeItemGroup*>( iter->m_pTreeItemBase.get() );
          ar & *pGroup;
        }
        break;
      }
    }
  }

  template<typename Archive>
  void load( Archive& ar, const unsigned int version ) {
    ar & boost::serialization::base_object<TreeItemResources>(*this);
    vMembers_t::size_type n;
    ar & n;
    for ( vMembers_t::size_type ix = 0; ix < n; ++ix ) {
      unsigned int type;
      ar & type;
      switch ( type ) {
        case IdGroup:
        {
          TreeItemGroup* pGroup = AddTreeItem<TreeItemGroup,IdTreeItemType>( "Group", IdGroup );
          ar & *pGroup;
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
