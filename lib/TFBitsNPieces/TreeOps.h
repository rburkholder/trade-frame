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

#pragma once


// 20151207  code originated in simulant

#include <map>

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

//#include <wx/wx.h>
#include <wx/defs.h>
#include <wx/treectrl.h>
//#include <wx/event.h>
#include <wx/bitmap.h>
#include <wx/icon.h>
#include <wx/string.h>

#include "TreeOpsItems.h"

#define SYMBOL_TREEOPS_STYLE wxTR_HAS_BUTTONS | wxTR_SINGLE
#define SYMBOL_TREEOPS_TITLE _("Tree Ops")
#define SYMBOL_TREEOPS_IDNAME ID_TREEOPS
#define SYMBOL_TREEOPS_SIZE wxSize(100, 200)
#define SYMBOL_TREEOPS_POSITION wxDefaultPosition

namespace ou {
namespace tf {  
  
class TreeOps: public wxTreeCtrl {    // http://docs.wxwidgets.org/trunk/classwx_tree_ctrl.html
  //DECLARE_DYNAMIC_CLASS( TreeOps )
  friend class boost::serialization::access;
public:
  
  typedef TreeItemBase::pTreeItemBase_t pTreeItemBase_t;
  
  TreeOps();
  TreeOps( 
          wxWindow* parent, 
          wxWindowID id = SYMBOL_TREEOPS_IDNAME, 
          const wxPoint& pos = SYMBOL_TREEOPS_POSITION, 
          const wxSize& size = SYMBOL_TREEOPS_SIZE, 
          long style = SYMBOL_TREEOPS_STYLE );

  bool Create( 
    wxWindow* parent, 
          wxWindowID id = SYMBOL_TREEOPS_IDNAME, 
          const wxPoint& pos = SYMBOL_TREEOPS_POSITION, 
          const wxSize& size = SYMBOL_TREEOPS_SIZE, 
          long style = SYMBOL_TREEOPS_STYLE );

  ~TreeOps();
  
  typedef boost::signals2::signal<void (const wxTreeItemId&)> signalChanging_t;
  typedef signalChanging_t::slot_type slotChanging_t;
  signalChanging_t signalChanging;
  
  void PopulateResources( TreeItemResources& resources );
  
  void SetRoot( pTreeItemBase_t pTreeItemRoot ) { 
    m_pTreeItemRoot = pTreeItemRoot; 
    assert( 0 == m_mapDecoder.size() );
    m_mapDecoder.insert( mapDecoder_t::value_type( pTreeItemRoot->GetTreeItemId(), pTreeItemRoot ) );
  }
  
  pTreeItemBase_t GetRoot( void ) {
    return m_pTreeItemRoot;
  }
  
  void Add( const wxTreeItemId& id, pTreeItemBase_t pTreeItemBase );
  void Delete( wxTreeItemId id );
  
  template<typename RootItemType>
  void Save( boost::archive::text_oarchive& oa) {
    TreeItemBase* pBase = m_pTreeItemRoot.get();
    const RootItemType* p = dynamic_cast<RootItemType*>( pBase );
    oa & *p;
  }
  
  template<typename RootItemType>
  void Load( boost::archive::text_iarchive& ia) {
    TreeItemBase* pBase = m_pTreeItemRoot.get();
    RootItemType* p = dynamic_cast<RootItemType*>( pBase );
    ia & *p;
  }
  
  wxString GetInput( const wxString& sPrompt, const wxString& sDefault );
  
protected:

  enum {
    ID_Null = wxID_HIGHEST,
    ID_TREEOPS
  };

  pTreeItemBase_t m_pTreeItemRoot; // root item tracked here for serialization starting at the root
  
private:
  
  wxTreeItemId m_idOld;
  
  typedef std::map<void*,pTreeItemBase_t> mapDecoder_t;  // void* is from wxTreeItemId
  mapDecoder_t m_mapDecoder;
  
  void Init();
  void CreateControls();

  void HandleContextMenu( wxTreeEvent& event );
  void HandleSelectionChanged( wxTreeEvent& event );
  void HandleSelectionChanging( wxTreeEvent& event );
  void HandleItemActivated( wxTreeEvent& event );
  void HandleItemDeleted( wxTreeEvent& event );
  
  void RemoveSelectOld( void );

  wxBitmap GetBitmapResource( const wxString& name );
  wxIcon GetIconResource( const wxString& name );
  static bool ShowToolTips( void ) { return true; };
  
};

} // namespace tf
} // namespace ou
