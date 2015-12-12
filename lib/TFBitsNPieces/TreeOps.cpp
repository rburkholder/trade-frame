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

// started December 7, 2015, 9:39 PM

#include <vector>

#include <wx/wx.h>

#include "TreeOps.h"

namespace ou {
namespace tf {

IMPLEMENT_DYNAMIC_CLASS( TreeOps, wxTreeCtrl )

struct TreeItemResources {  // used by inheritors of TreeItemBase

  TreeOps& tree;  // used for assigning the popup, plus other base class functions, eg for binding, etc

  TreeItemResources( TreeOps& tree ): tree( tree ) {}
};

class TreeItemBase {
  friend class boost::serialization::access;
public:
  
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
    std::string sLabel( m_pResources->tree.GetItemText( m_id ) );
    ar & sLabel;
  }
  
  template<typename Archive>
  void load( Archive& ar, const unsigned int version ) {
    std::string sLabel;
    ar & sLabel;
    m_pResources->tree.SetItemText( m_id, sLabel );
  }
  
  BOOST_SERIALIZATION_SPLIT_MEMBER()
  
};

void TreeItemBase::AddMember( unsigned int type, wxTreeItemId id, pTreeItemBase_t p ) {
  assert( 0 != id.GetID() );
  assert( 0 != p.get() );
  m_vMembers.push_back( member_t( type, id.GetID(), p ) );
  m_mapMembers.insert( mapMembers_t::value_type( id.GetID(), p ) );
}

void TreeItemBase::DeleteMember( wxTreeItemId id ) {
  
  struct matchId {
    bool operator()( const TreeItemBase::member_t& member ) const { return member.m_void == m_v; }
    matchId( void* v ): m_v( v ) {};
    void* m_v;
  };
  
  mapMembers_t::const_iterator iterMap = m_mapMembers.find( id.GetID() );
  if ( m_mapMembers.end() != iterMap ) {
    m_mapMembers.erase( iterMap );
  }
  else assert( 0 );
  vMembers_t::iterator iterVec = std::find_if( m_vMembers.begin(), m_vMembers.end(), matchId( id.GetID() ) );
  assert( m_vMembers.end() != iterVec );
  m_vMembers.erase( iterVec );
}

void TreeItemBase::HandleRename( wxCommandEvent& event ) { 
  Rename();
}

void TreeItemBase::Rename( const wxString& sPrompt ) { 
  Rename( sPrompt, m_pResources->tree.GetItemText( m_id ) );
}

void TreeItemBase::Rename( const wxString& sPrompt, const wxString& sDefault ) { 
  wxTextEntryDialog* p = new wxTextEntryDialog( &m_pResources->tree, sPrompt, sPrompt, sDefault );
  if ( wxID_OK == p->ShowModal() ) {
    m_pResources->tree.SetItemText( m_id, p->GetValue() );
  }
}

wxTreeItemId TreeItemBase::AppendSubItem( const std::string& sLabel ) {
  wxTreeItemId id = m_pResources->tree.AppendItem( m_id, sLabel );
  m_pResources->tree.EnsureVisible( id );
  return id;
}

pTreeItemBase_t TreeItemBase::AppendSubItem( wxTreeItemId id, TreeItemBase* p ) {
  pTreeItemBase_t pTreeItemBase( p );
  m_pResources->tree.Add( id, pTreeItemBase );
  return pTreeItemBase;
}

// ====================================
    
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

void TreeItemGroup::ShowContextMenu( void ) {
  wxMenu* pMenu = new wxMenu();
  pMenu->Append( MIAddSubGroup, "Add Sub &Group" );
  pMenu->Bind( wxEVT_COMMAND_MENU_SELECTED, &TreeItemGroup::HandleAddGroup, this, MIAddSubGroup );
  pMenu->Append( MIRename, "&Rename" );
  pMenu->Bind( wxEVT_COMMAND_MENU_SELECTED, &TreeItemBase::HandleRename, this, MIRename );
  pMenu->Append( MIDelete, "Delete" );
  pMenu->Bind( wxEVT_COMMAND_MENU_SELECTED, &TreeItemGroup::HandleDelete, this, MIDelete );
  m_pResources->tree.PopupMenu( pMenu );
}

void TreeItemGroup::HandleAddGroup( wxCommandEvent& event ) { 
  //TreeItemGroup* p = AddGroup();
  TreeItemGroup* p = AddTreeItem<TreeItemGroup>( "Group", IdGroup );
  p->Rename();
}

void TreeItemGroup::HandleDelete( wxCommandEvent& event ) {
  std::cout << "TreeItemGroup Delete" << std::endl;
  m_pResources->tree.Delete( this->m_id );
}

// ================

class TreeItemRoot: public TreeItemGroup {
  friend class boost::serialization::access;
public:

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

void TreeItemRoot::ShowContextMenu( void ) {
  //wxMenu* pMenu = new wxMenu( "Surfaces");
  wxMenu* pMenu = new wxMenu();
  pMenu->Append( MIAddGroup, "&Add Group" );
  pMenu->Bind( wxEVT_COMMAND_MENU_SELECTED, &TreeItemRoot::HandleAddGroup, this, MIAddGroup );
  m_pResources->tree.PopupMenu( pMenu );
}

void TreeItemRoot::HandleAddGroup( wxCommandEvent& event ) {
  TreeItemGroup* pGroup = TreeItemGroup::AddTreeItem<TreeItemGroup>( "Group", TreeItemGroup::IdGroup );
  pGroup->Rename();
}

// ================
TreeOps::TreeOps() {
  Init();
}

TreeOps::TreeOps( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) {
  Init();
  Create(parent, id, pos, size, style);
}

bool TreeOps::Create( wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style ) {
    
  SetExtraStyle(wxWS_EX_VALIDATE_RECURSIVELY);
  wxTreeCtrl::Create( parent, id, pos, size, style );

  CreateControls();
  if (GetSizer()) {
    GetSizer()->SetSizeHints(this);
  }
  Centre();
  return true;
}

TreeOps::~TreeOps() {
  // m_resources.pAudio.reset();  // implied now
}

void TreeOps::Init() {
  m_pResources.reset( new TreeItemResources( *this ) );
  //m_resources.pAudio = 0; // implied now
  //m_resources.sCurrentPath = std::string( ::wxGetCwd() );
}

void TreeOps::CreateControls() {    
  
  TreeOps* item = this;
  
  wxTreeCtrl::Bind( wxEVT_TREE_ITEM_MENU, &TreeOps::HandleContextMenu, this );
  wxTreeCtrl::Bind( wxEVT_TREE_SEL_CHANGED, &TreeOps::HandleSelectionChanged, this );
  wxTreeCtrl::Bind( wxEVT_TREE_SEL_CHANGING, &TreeOps::HandleSelectionChanging, this );
  wxTreeCtrl::Bind( wxEVT_TREE_ITEM_ACTIVATED, &TreeOps::HandleItemActivated, this );
  wxTreeCtrl::Bind( wxEVT_TREE_DELETE_ITEM, &TreeOps::HandleItemDeleted, this );
  
  wxTreeItemId id = wxTreeCtrl::AddRoot( "Root" );  // can be renamed
  m_pTreeItemRoot.reset( new TreeItemRoot( id, m_pResources ) );
  m_mapDecoder.insert( mapDecoder_t::value_type( id.GetID(), m_pTreeItemRoot ) );
  
}

void TreeOps::Add( const wxTreeItemId& id, pTreeItemBase_t pTreeItemBase ) {
  m_mapDecoder.insert( mapDecoder_t::value_type( id.GetID(), pTreeItemBase ) );
}

void TreeOps::Delete( wxTreeItemId id ) {
  if ( 0 == GetChildrenCount( id ) ) {
    
    // need to detect that it is deleting itself, and that it is currently selected
    if ( m_idOld.IsOk() ) {
      if ( m_idOld = id ) {
        //m_mapDecoder[ m_idOld ]->RemoveSelected( m_guiElements );
      }
    }
    
    wxTreeItemId idParent = wxTreeCtrl::GetItemParent( id );
    assert( idParent.IsOk() );
    mapDecoder_t::iterator iterParent = m_mapDecoder.find( idParent.GetID() );
    assert( m_mapDecoder.end() != iterParent );
    iterParent->second->DeletingChild( id );
    
    mapDecoder_t::iterator iterChild = m_mapDecoder.find( id.GetID() );
    assert( m_mapDecoder.end() != iterChild );
    
    wxTreeCtrl::Delete( id );
    m_idOld.Unset();
    m_mapDecoder.erase( iterChild );
  }
  else {
    std::cout << "item has children" << std::endl;
  }
}

void TreeOps::HandleContextMenu( wxTreeEvent& event ) {
  m_mapDecoder[ event.GetItem().GetID() ]->ShowContextMenu();
}

void TreeOps::HandleSelectionChanged( wxTreeEvent& event ) {
//  std::cout << "HandleSelectionChanged " << event.GetItem().GetID() << std::endl;
  m_idOld = event.GetItem();
  //m_mapDecoder[ m_idOld.GetID() ]->SetSelected( m_guiElements );
  
}

void TreeOps::RemoveSelectOld( void ) {
  //std::cout << "HandleSelectionChanging " << event.GetItem().GetID() << std::endl;
  //if ( m_idOld.IsOk() ) m_mapDecoder[ m_idOld ]->RemoveSelected( m_guiElements );
  m_idOld.Unset();
}

void TreeOps::HandleSelectionChanging( wxTreeEvent& event ) {
  RemoveSelectOld();
}

void TreeOps::HandleItemActivated( wxTreeEvent& event ) {
  //std::cout << "HandleItemActivated" << std::endl;
}

void TreeOps::HandleItemDeleted( wxTreeEvent& event ) {
  //std::cout << "HandleItemDeleted" << std::endl;
}

wxBitmap TreeOps::GetBitmapResource( const wxString& name ) {
  wxUnusedVar(name);
  return wxNullBitmap;
}

wxIcon TreeOps::GetIconResource( const wxString& name ) {
  wxUnusedVar(name);
  return wxNullIcon;
}

void TreeOps::Save( boost::archive::text_oarchive& oa ) {
  //size_t n( m_pResources->vpSceneManager.size() );
  //oa & n;

  TreeItemBase* pBase = m_pTreeItemRoot.get();
  const TreeItemRoot* p = dynamic_cast<TreeItemRoot*>( pBase );
  oa & *p;
}

void TreeOps::Load( boost::archive::text_iarchive& ia ) {
  //size_t n;
  //ia & n;
  //assert( m_pResources->vpSceneManager.size() == n );

  TreeItemBase* pBase = m_pTreeItemRoot.get();
  TreeItemRoot* p = dynamic_cast<TreeItemRoot*>( pBase );
  ia & *p;
}

} // namespace tf
} // namespace ou