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


// 20151207  Need to generalize this, so can be used elsewhere, 
//   considering we are stealing the code from simulant

#include <map>

#include <boost/shared_ptr.hpp>

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

//#include <boost/phoenix/bind/bind_member_function.hpp>
//#include <boost/phoenix/core/argument.hpp>

//#include <wx/wx.h>
#include <wx/treectrl.h>
//#include <wx/panel.h>
#include <wx/event.h>
#include <wx/bitmap.h>
#include <wx/icon.h>

#define SYMBOL_TREEOPS_STYLE wxTR_HAS_BUTTONS | wxTR_SINGLE
#define SYMBOL_TREEOPS_TITLE _("Tree Ops")
#define SYMBOL_TREEOPS_IDNAME ID_TREEOPS
#define SYMBOL_TREEOPS_SIZE wxSize(100, 200)
#define SYMBOL_TREEOPS_POSITION wxDefaultPosition

namespace ou {
namespace tf {  

class TreeItemBase;  // base class for different items for populating the tree
class TreeItemResources;  // common set of resources throughout the tree items

typedef boost::shared_ptr<TreeItemBase> pTreeItemBase_t;
typedef boost::shared_ptr<TreeItemResources> pTreeItemResources_t;

class TreeOps: public wxTreeCtrl {
  DECLARE_DYNAMIC_CLASS( TreeOps )
  friend class boost::serialization::access;
public:
  
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
  
  void Add( const wxTreeItemId& id, pTreeItemBase_t pTreeItemBase );
  void Delete( wxTreeItemId id );
  
  void Save( boost::archive::text_oarchive& oa);
  void Load( boost::archive::text_iarchive& ia);
  
protected:
private:
  
  enum {
    ID_Null = wxID_HIGHEST,
    ID_TREEOPS
  };

  wxTreeItemId m_idOld;
  
  pTreeItemBase_t m_pTreeItemRoot; // root item tracked here for serialization root
  pTreeItemResources_t m_pResources;
  
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