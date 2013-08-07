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

class ModelOrder: public ModelBase<ModelOrder> {
public:

  typedef ou::tf::OrderManager OrderManager;
  typedef OrderManager::idOrder_t idOrder_t;
  typedef OrderManager::pOrder_t pOrder_t;

  struct DataViewItemOrder: public DataViewItem<pOrder_t::element_type> {
    DataViewItemOrder( shared_ptr& ptr )
      : DataViewItem<pOrder_t::element_type>( ptr ) { ixType = eOrder; };
    void AssignFirstColumn( wxVariant& variant ) const {
      variant = (std::string&) m_ptr->GetRow().idOrder;
    }
  };

  typedef std::map<idOrder_t, DataViewItemOrder> mapItems_t;

  ModelOrder(void);
  ~ModelOrder(void);

protected:
private:
};

} // namespace tf
} // namespace ou
