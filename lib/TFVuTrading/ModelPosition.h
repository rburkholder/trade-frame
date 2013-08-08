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

class ModelPosition: public ModelBase<ModelPosition> {
public:

  typedef ou::tf::PortfolioManager PortfolioManager;
  typedef PortfolioManager::idPosition_t idPosition_t;
  typedef PortfolioManager::pPosition_t pPosition_t;

  struct DataViewItemPosition: public DataViewItem<pPosition_t::element_type> {
    DataViewItemPosition( shared_ptr& ptr )
      : DataViewItem<pPosition_t::element_type>( ptr ) { ixType = ePosition; };
    void AssignFirstColumn( wxVariant& variant ) const {
      variant = (std::string&) m_ptr->GetRow().idPosition;
    }
  };

  typedef std::map<void*, DataViewItemPosition*> mapItems_t;

  ModelPosition(void);
  ~ModelPosition(void);

protected:
private:
};

} // namespace tf
} // namespace ou
