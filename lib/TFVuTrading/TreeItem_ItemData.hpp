/************************************************************************
 * Copyright(c) 2025, One Unified. All rights reserved.                 *
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
 * File:    TreeItem_ItemData.hpp
 * Author:  raymond@burkholder.net
 * Project: TFVuTrading
 * Created: April 2, 2025 20:15:01
 */

#pragma once

#include <wx/treectrl.h>

#include "TreeItem.hpp"

namespace ou { // One Unified
namespace tf { // TradeFrame

class CustomItemData_Base: public wxTreeItemData {
public:

  CustomItemData_Base( TreeItem* pTreeItem ) // self?
  : wxTreeItemData(), m_pTreeItem( pTreeItem ) {}

  ~CustomItemData_Base() {
    // assumes binds are cleared as well
    if ( m_pTreeItem ) {
      delete m_pTreeItem;
      m_pTreeItem = nullptr;
    }
  }

  TreeItem* GetTreeItem() { return m_pTreeItem; }

protected:
private:
  TreeItem* m_pTreeItem;
};

} // namespace tf
} // namespace ou
