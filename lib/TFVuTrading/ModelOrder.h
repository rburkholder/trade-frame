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

class ModelOrder: public ModelBase {
public:

  typedef ou::tf::OrderManager OrderManager;
  typedef OrderManager::idOrder_t idOrder_t;
  typedef OrderManager::pOrder_t pOrder_t;

  struct DataViewItemOrder: public ModelBase::DataViewItem<pOrder_t::element_type> {
    void GetFirstColumn( wxVariant& variant ) const {
      variant = (std::string&) m_ptr->GetRow().idOrder;
    }
  };

  ModelOrder(void);
  ~ModelOrder(void);

protected:
private:
};

} // namespace tf
} // namespace ou
