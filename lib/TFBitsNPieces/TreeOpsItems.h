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

#include "FirstOrDefaultCombiner.h"

namespace ou {
namespace tf { 
  
class TreeItemBase;
  
struct TreeItemResources {  // used by inheritors of TreeItemBase
  
  typedef boost::signals2::signal<void (const wxTreeItemId&, const std::string&)> signalSetItemText_t;
  typedef signalSetItemText_t::slot_type slotSetItemText_t;
  signalSetItemText_t signalSetItemText;
  
  typedef boost::signals2::signal<wxString (const wxTreeItemId&), FirstOrDefault<wxString> > signalGetItemText_t;
  typedef signalGetItemText_t::slot_type slotGetItemText_t;
  signalGetItemText_t signalGetItemText;
  
  typedef boost::signals2::signal<wxString (const wxString&, const wxString&), FirstOrDefault<wxString> > signalGetInput_t;
  typedef signalGetInput_t::slot_type slotGetInput_t;
  signalGetInput_t signalGetInput;
  
  typedef boost::shared_ptr<TreeItemBase> pTreeItemBase_t;
  typedef boost::signals2::signal<void (const wxTreeItemId&, pTreeItemBase_t)> signalAdd_t;
  typedef signalAdd_t::slot_type slotAdd_t;
  signalAdd_t signalAdd;
  
  typedef boost::signals2::signal<void (const wxTreeItemId&)> signalDelete_t;
  typedef signalDelete_t::slot_type slotDelete_t;
  signalDelete_t signalDelete;
  
  typedef boost::signals2::signal<void (wxMenu*)> signalPopupMenu_t;
  typedef signalPopupMenu_t::slot_type slotPopupMenu_t;
  signalPopupMenu_t signalPopupMenu;
  
  typedef boost::signals2::signal<wxTreeItemId (const wxTreeItemId&, const std::string&), FirstOrDefault<wxTreeItemId> > signalAppendItem_t;
  typedef signalAppendItem_t::slot_type slotAppendItem_t;
  signalAppendItem_t signalAppendItem;
  
  typedef boost::signals2::signal<void (const wxTreeItemId&)> signalEnsureVisible_t;
  typedef signalEnsureVisible_t::slot_type slotEnsureVisible_t;
  signalEnsureVisible_t signalEnsureVisible;
  
};

// ===========

class TreeItemBase {
  friend class boost::serialization::access;
public:
  
  typedef boost::shared_ptr<TreeItemBase> pTreeItemBase_t;
  
  TreeItemBase( wxTreeItemId id, TreeItemResources& resources ): 
    m_id( id ), m_baseResources( resources ), m_pMenu( 0 )
    {}
  //TreeItemBase( wxTreeItemId id ): m_id( id ) {}
  virtual ~TreeItemBase( void ) {
    if ( 0 != m_pMenu ) {
      delete m_pMenu;
      m_pMenu = 0;
    }
  }
  
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
  
  TreeItemResources& m_baseResources;
  
  typedef std::map<void*,pTreeItemBase_t> mapMembers_t;  // void* from wxTreeItemId, tracks owned items for access
  mapMembers_t m_mapMembers; 
  
  struct member_t {
    unsigned int m_type;
    void* m_void;
    pTreeItemBase_t m_pTreeItemBase;
    member_t( unsigned int type, void* void_, pTreeItemBase_t p ): m_type( type ), m_void( void_ ), m_pTreeItemBase( p ) {};
  };
  
  wxMenu* m_pMenu;  // context menu

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
    TreeItem* p = new TreeItem( id, m_baseResources );
    assert( 0 != p );
    pTreeItemBase_t pTreeItemBase = AppendSubItem( id, p );
    assert( 0 != pTreeItemBase.get() );
    AddMember( idType, id, pTreeItemBase );
    return p;
  }
  
  template<typename TreeItem, typename id_t, typename Resources>  
  TreeItem* AddTreeItem( const std::string& sLabel, id_t idType, Resources& resources ) { 
    wxTreeItemId id = AppendSubItem( sLabel );
    TreeItem* p = new TreeItem( id, m_baseResources, resources );
    assert( 0 != p );
    pTreeItemBase_t pTreeItemBase = AppendSubItem( id, p );
    assert( 0 != pTreeItemBase.get() );
    AddMember( idType, id, pTreeItemBase );
    return p;
  }
  
private:
  
  template<typename Archive>
  void save( Archive& ar, const unsigned int version ) const {
    //ar & boost::serialization::base_object<const TreeItemBase>(*this);
    //wxString s = m_pResources->signalGetItemText( m_id );
    //std::string sLabel( m_pResources->tree.GetItemText( m_id ) );
    std::string sLabel( m_baseResources.signalGetItemText( m_id ) );
    ar & sLabel;
  }
  
  template<typename Archive>
  void load( Archive& ar, const unsigned int version ) {
    //ar & boost::serialization::base_object<TreeItemBase>(*this);
    std::string sLabel;
    ar & sLabel;
    //m_pResources->tree.SetItemText( m_id, sLabel );
    m_baseResources.signalSetItemText( m_id, sLabel );
  }
  
  BOOST_SERIALIZATION_SPLIT_MEMBER()
  
};

} // namespace tf
} // namespace ou
