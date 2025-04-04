/************************************************************************
 * Copyright(c) 2022, One Unified. All rights reserved.                 *
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

/*
 * File:    Item.hpp
 * Author:  raymond@burkholder.net
 * Project: TFVuTrading
 * Created: March 31, 2022 09:50
 */

#pragma once

#include <string>
#include <functional>

#include <wx/treebase.h>

class wxMenu;
class wxTreeCtrl;

namespace ou { // One Unified
namespace tf { // TradeFrame

class CustomItemData_Base;

class TreeItem {
public:

  using fOnClick_t = std::function<void(TreeItem*)>;       // left click
  using fOnBuildPopUp_t = std::function<void(TreeItem*)>;  // right click
  using fOnDeleted_t = std::function<void()>;
  using fCustomItemData_Factory_t = std::function<CustomItemData_Base*( TreeItem* )>;

  TreeItem( wxTreeCtrl*, const std::string& ); // only used to attach root item
  TreeItem( wxTreeCtrl*, const std::string&, fCustomItemData_Factory_t&& ); // only used to attach root item
  ~TreeItem();

  void SetOnClick( fOnClick_t&& fOnClick ) { m_fOnClick = std::move( fOnClick ); }
  void SetOnBuildPopUp( fOnBuildPopUp_t&& fOnBuildPopUp ) { m_fOnBuildPopUp = std::move( fOnBuildPopUp ); }
  void SetOnDeleted( fOnDeleted_t&& fOnDeleted ) { m_fOnDeleted = std::move( fOnDeleted ); }

  TreeItem* AppendChild( const std::string& );  // all tree build operations
  TreeItem* AppendChild( const std::string&, fOnClick_t&& );  // all tree build operations
  TreeItem* AppendChild( const std::string&, fOnClick_t&&, fOnBuildPopUp_t&& );  // all tree build operations

  void SortChildren();

  void Delete();
  void UpdateText( const std::string& );

  void NewMenu();
  void AppendMenuItem( const std::string& sText, fOnClick_t&& );

  static void Bind( wxWindow*, wxTreeCtrl* );  // Initialize event handling
  //static void UnBind( wxWindow*, wxTreeCtrl* ); // lambdas need to be methods

protected:
private:

  wxTreeItemId m_idSelf;
  wxTreeItemId m_idParent;

  wxMenu* m_pMenuPopup;
  wxTreeCtrl* m_pTreeCtrl;

  fOnClick_t m_fOnClick;
  fOnBuildPopUp_t m_fOnBuildPopUp;
  fOnDeleted_t m_fOnDeleted;
  fCustomItemData_Factory_t m_fCustomItemData_Factory;

  TreeItem( wxTreeCtrl*, wxTreeItemId idParent, const std::string& ); // used in AppendChild

  void HandleTreeEventItemChanged();
  void HandleTreeEventItemMenu();

  void Deleted( const wxTreeItemId& );
};

} // namespace tf
} // namespace ou
