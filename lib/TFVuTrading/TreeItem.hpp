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
  using fIterateChildren_t = std::function<bool( TreeItem* )>; // false to stop

  TreeItem( wxTreeCtrl*, const std::string& ); // only used to attach root item
  TreeItem( wxTreeCtrl*, const std::string&, fCustomItemData_Factory_t&& ); // only used to attach root item
  ~TreeItem();

  TreeItem* GetParent() { return m_ptiParent; }

  void SetOnClick( fOnClick_t&& fOnClick ) { m_fOnClick = std::move( fOnClick ); }
  void SetOnBuildPopUp( fOnBuildPopUp_t&& fOnBuildPopUp ) { m_fOnBuildPopUp = std::move( fOnBuildPopUp ); }
  void SetOnDeleted( fOnDeleted_t&& fOnDeleted ) { m_fOnDeleted = std::move( fOnDeleted ); }

  TreeItem* AppendChild( const std::string& );  // all tree build operations
  TreeItem* AppendChild( const std::string&, fOnClick_t&& );  // all tree build operations
  TreeItem* AppendChild( const std::string&, fOnClick_t&&, fOnBuildPopUp_t&& );  // all tree build operations
  TreeItem* AppendChild( const std::string&, fOnClick_t&&, fOnBuildPopUp_t&&, fCustomItemData_Factory_t&& );  // all tree build operations

  void Expand();

  void SortChildren();

  void IterateChildren( fIterateChildren_t&& );

  void Delete();
  void DeleteChildren();
  void UpdateText( const std::string& );
  std::string GetText() const;

  void NewMenu();
  void AppendMenuItem( const std::string& sText, fOnClick_t&& );

  CustomItemData_Base* CustomItemData();

  static void Bind( wxWindow*, wxTreeCtrl* );  // Initialize event handling
  //static void UnBind( wxWindow*, wxTreeCtrl* ); // lambdas need to be methods

protected:
private:

  wxTreeCtrl* m_pTreeCtrl;

  TreeItem* m_ptiParent;
  wxTreeItemId m_idSelf;

  wxMenu* m_pMenuPopup;

  fOnClick_t m_fOnClick;
  fOnBuildPopUp_t m_fOnBuildPopUp;
  fOnDeleted_t m_fOnDeleted;

  TreeItem( wxTreeCtrl*, TreeItem* ptiParent, const std::string& ); // used in AppendChild
  TreeItem( wxTreeCtrl*, TreeItem* ptiParent, const std::string&, fCustomItemData_Factory_t&& ); // used in AppendChild

  void HandleTreeEventItemChanged();
  void HandleTreeEventItemMenu();

  void Deleted( const wxTreeItemId& );
};

} // namespace tf
} // namespace ou
