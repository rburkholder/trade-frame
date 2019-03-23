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

// started December 21, 2015, 10:22 PM

#pragma once

#include <TFBitsNPieces/TreeOps.h>

#include "TreeItem.h"

class TreeOps: public ou::tf::TreeOps {
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
  virtual ~TreeOps();

  void PopulateResources( Resources& resources );

  template<typename RootItemType>
  void Save( boost::archive::text_oarchive& oa) {
    ou::tf::TreeOps::Save<RootItemType>( oa );
  }

  template<typename RootItemType>
  void Load( boost::archive::text_iarchive& ia) {
    ou::tf::TreeOps::Load<RootItemType>( ia );
  }



protected:
private:

};
