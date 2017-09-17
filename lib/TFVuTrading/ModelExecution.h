/************************************************************************
 * Copyright(c) 2011, One Unified. All rights reserved.                 *
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

#pragma once

#include <set>

#include <TFTrading/OrderManager.h>

#include "ModelBase.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

class ModelExecution: public ModelBase<ModelExecution> {
public:

  typedef ou::tf::OrderManager OrderManager;
  typedef OrderManager::idExecution_t idExecution_t;
  typedef OrderManager::pExecution_t pExecution_t;

  struct DataViewItemExecution: public DataViewItem<pExecution_t::element_type> {
    DataViewItemExecution( shared_ptr& ptr )
      : DataViewItem<pExecution_t::element_type>( EMTExecution, ptr ) {};
    void AssignFirstColumn( wxVariant& variant ) /* const */ {
      variant = (std::string&) GetPtr()->GetRow().idExecution;
    }
  };
  
  struct wxDataViewItem_Execution: public wxDataViewItem_typed<DataViewItemExecution> {};

  //typedef std::map<void*, DataViewItemExecution*> mapItems_t;
  typedef std::set<DataViewItemExecution*> setItems_t;
  
  ModelExecution(void);
  ~ModelExecution(void);

};

} // namespace tf
} // namespace ou
