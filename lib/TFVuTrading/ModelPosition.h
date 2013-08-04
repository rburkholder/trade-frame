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

#include <TFTrading/PortfolioManager.h>

#include "ModelBase.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

class ModelPosition: public ModelBase {
public:

  typedef ou::tf::PortfolioManager PortfolioManager;
  typedef PortfolioManager::idPosition_t idPosition_t;
  typedef PortfolioManager::pPosition_t pPosition_t;

  struct DataViewItemPosition: public ModelBase::DataViewItem<pPosition_t::element_type> {
    void GetFirstColumn( wxVariant& variant ) const {
      variant = (std::string&) m_ptr->GetRow().idPosition;
    }
  };

  ModelPosition(void);
  ~ModelPosition(void);

protected:
private:
};

} // namespace tf
} // namespace ou
