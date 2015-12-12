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

// started December 12, 2015, 2:11 PM

#pragma once

#include <map>

#include <boost/signals2.hpp>

#include <boost/shared_ptr.hpp>

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

#include <wx/treectrl.h>
#include <wx/string.h>
#include <wx/menu.h>

namespace ou {
namespace tf { 
  
template<typename T>
struct FirstOrDefault {  // a combiner used for signals with return values
  typedef T result_type;
  template<typename InputIterator>
  T operator()( InputIterator first, InputIterator last ) const {
    if (first==last) return T();
    return *first;
  }
};

class TreeItemBase;
  
// can be inherited 
struct TreeItemResources {  // used by inheritors of TreeItemBase
  typedef boost::shared_ptr<TreeItemResources> pTreeItemResources_t;
  typedef boost::shared_ptr<TreeItemBase> pTreeItemBase_t;
  
  typedef boost::signals2::signal<void (wxTreeItemId, const std::string&)> signalSetItemText_t;
  typedef signalSetItemText_t::slot_type slotSetItemText_t;
  signalSetItemText_t signalSetItemText;
  
  typedef boost::signals2::signal<wxString (wxTreeItemId), FirstOrDefault<wxString> > signalGetItemText_t;
  typedef signalGetItemText_t::slot_type slotGetItemText_t;
  signalGetItemText_t signalGetItemText;
  
  typedef boost::signals2::signal<wxString (const wxString&, const wxString&), FirstOrDefault<wxString> > signalGetInput_t;
  typedef signalGetInput_t::slot_type slotGetInput_t;
  signalGetInput_t signalGetInput;
  
  typedef boost::signals2::signal<void (const wxTreeItemId&, pTreeItemBase_t)> signalAdd_t;
  typedef signalAdd_t::slot_type slotAdd_t;
  signalAdd_t signalAdd;
  
  typedef boost::signals2::signal<void (const wxTreeItemId&)> signalDelete_t;
  typedef signalDelete_t::slot_type slotDelete_t;
  signalDelete_t signalDelete;
  
  typedef boost::signals2::signal<void (wxMenu*)> signalPopupMenu_t;
  typedef signalPopupMenu_t::slot_type slotPopupMenu_t;
  signalPopupMenu_t signalPopupMenu;
};

// ===========

class TreeItemBase {
  friend class boost::serialization::access;
public:
  
  typedef boost::shared_ptr<TreeItemBase> pTreeItemBase_t;
  typedef TreeItemResources::pTreeItemResources_t pTreeItemResources_t;
  
  TreeItemBase( wxTreeItemId id, pTreeItemResources_t pResources ): m_id( id ), m_pResources( pResources ) {}
  virtual ~TreeItemBase( void ) {}
  
  virtual void ShowContextMenu( void ) {}
//  virtual void SetSelected( CommonGuiElements& ) {}
//  virtual void RemoveSelected( CommonGuiElements& ) {}
  virtual void DeletingChild( wxTreeItemId id ) {};
  wxTreeItemId GetTreeItemId( void ) { return m_id; }
  
  void HandleRename( wxCommandEvent& event );
  
  void Rename( const wxString& sPrompt, const wxString& sDefault );
  void Rename( const wxString& sPrompt = "Change Text:" );
  
protected:
  
  wxTreeItemId m_id;  // identifier of this part of the tree control
  
  pTreeItemResources_t m_pResources;
  
  typedef std::map<void*,pTreeItemBase_t> mapMembers_t;  // void* from wxTreeItemId, tracks owned items for access
  mapMembers_t m_mapMembers; 
  
  struct member_t {
    unsigned int m_type;
    void* m_void;
    pTreeItemBase_t m_pTreeItemBase;
    member_t( unsigned int type, void* void_, pTreeItemBase_t p ): m_type( type ), m_void( void_ ), m_pTreeItemBase( p ) {};
  };

  typedef std::vector<member_t> vMembers_t; // tracks ordered list for serialization
  vMembers_t m_vMembers;

  wxTreeItemId AppendSubItem( const std::string& sLabel ); // add the visual menu item
  pTreeItemBase_t  AppendSubItem( wxTreeItemId id, TreeItemBase* p );  // add the associated real object
  
  // convenience member functions for derived classes
  void AddMember( unsigned int type, wxTreeItemId id, pTreeItemBase_t p ); // stuff to be serialized
  void DeleteMember( wxTreeItemId id ); // stuff not to be serialized
  
  template<typename TreeItem, typename id_t>  
  TreeItem* AddTreeItem( const std::string& sLabel, id_t idType ) { 
    wxTreeItemId id = AppendSubItem( sLabel );
    TreeItem* p = new TreeItem( id, m_pResources );
    assert( 0 != p );
    pTreeItemBase_t pTreeItemBase = AppendSubItem( id, p );
    assert( 0 != pTreeItemBase.get() );
    AddMember( idType, id, pTreeItemBase );
    return p;
  }

private:
  
  template<typename Archive>
  void save( Archive& ar, const unsigned int version ) const {
    //wxString s = m_pResources->signalGetItemText( m_id );
    //std::string sLabel( m_pResources->tree.GetItemText( m_id ) );
    std::string sLabel( m_pResources->signalGetItemText( m_id ) );
    ar & sLabel;
  }
  
  template<typename Archive>
  void load( Archive& ar, const unsigned int version ) {
    std::string sLabel;
    ar & sLabel;
    //m_pResources->tree.SetItemText( m_id, sLabel );
    m_pResources->signalSetItemText( m_id, sLabel );
  }
  
  BOOST_SERIALIZATION_SPLIT_MEMBER()
  
};
  
class TreeItemGroup: public TreeItemBase {
  friend class boost::serialization::access;
public:

  // deals with organizing groups of branches, eg:  master - act - scene
  TreeItemGroup( wxTreeItemId id, pTreeItemResources_t pResources ): TreeItemBase( id, pResources ) {}
  virtual ~TreeItemGroup( void ) {};

  virtual void ShowContextMenu( void );

protected:

  enum IdTreeItemType {
    IdGroup = 201, IdScene
  };

  TreeItemGroup* AddGroup( void );  // for TreeItemRoot

private:

  enum {
    ID_Null = wxID_HIGHEST,
    MIAddSubGroup, MIAddMusic, MIAddScene,
    MIDelete, MIRename
  };

  void HandleAddGroup( wxCommandEvent& event );
  //void HandleAddMusic( wxCommandEvent& event );
  //TreeItemMusic* AddMusic( void );

  //void HandleAddScene( wxCommandEvent& event );
  //TreeItemScene* AddScene( void );

  void HandleDelete( wxCommandEvent& event );

  template<typename Archive>
  void save( Archive& ar, const unsigned int version ) const {
    ar << boost::serialization::base_object<const TreeItemBase>(*this);
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
    ar & boost::serialization::base_object<TreeItemBase>(*this);
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

class TreeItemRoot: public TreeItemGroup {
  friend class boost::serialization::access;
public:
  
  typedef boost::shared_ptr<TreeItemRoot> pTreeItemRoot_t;

  TreeItemRoot( wxTreeItemId id, pTreeItemResources_t pResources ): TreeItemGroup( id, pResources ) {}
  virtual ~TreeItemRoot( void ) {}

  virtual void ShowContextMenu( void );

protected:
private:
  enum {
    ID_Null = wxID_HIGHEST,
    MIAddGroup
  };

  void HandleAddGroup( wxCommandEvent& event );

  template<typename Archive>
  void save( Archive& ar, const unsigned int version ) const {
    ar & boost::serialization::base_object<const TreeItemGroup>(*this);
  }

  template<typename Archive>
  void load( Archive& ar, const unsigned int version ) {
    ar & boost::serialization::base_object<TreeItemGroup>(*this);
  }

  BOOST_SERIALIZATION_SPLIT_MEMBER()

};

} // namespace tf
} // namespace ou
