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
      : DataViewItem<pExecution_t::element_type>( ptr ) { ixType = eExecution; };
    void AssignFirstColumn( wxVariant& variant ) const {
      variant = (std::string&) m_ptr->GetRow().idExecution;
    }
  };

  typedef std::map<void*, DataViewItemExecution*> mapItems_t;

  ModelExecution(void);
  ~ModelExecution(void);

};

} // namespace tf
} // namespace ou
