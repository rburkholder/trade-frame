/************************************************************************
 * Copyright(c) 2013, One Unified. All rights reserved.                 *
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

// Started 2013/07/30

#pragma once

#include <TFTrading/PortfolioManager.h>
#include <TFTrading/OrderManager.h>

#include "ModelBase.h"

#include "ModelPortfolio.h"
#include "ModelPosition.h"
#include "ModelOrder.h"
#include "ModelExecution.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

class ModelPortfolioPositionOrderExecution: public ModelBase { // model for tree of portfolio, position, order, execution
public:

  ModelPortfolioPositionOrderExecution(void);
  ~ModelPortfolioPositionOrderExecution(void);

  void LoadMasterPortfolio( void );

  ModelPortfolio* GetModelPortfolio( void ) { return m_pModelPortfolio; };
  ModelPosition* GetModelPosition( void ) { return m_pModelPosition; };
  ModelOrder* GetModelOrder( void ) { return m_pModelOrder; };
  ModelExecution* GetModelExecution( void ) { return m_pModelExecution; };

  unsigned int GetChildren(	const wxDataViewItem& item, wxDataViewItemArray& children	) const;
  void GetValue( wxVariant& variant, const wxDataViewItem& item, unsigned int col	) const;

protected:
private:

  typedef Portfolio::idPortfolio_t idPortfolio_t;

  ModelPortfolio* m_pModelPortfolio;
  ModelPosition* m_pModelPosition;
  ModelOrder* m_pModelOrder;
  ModelExecution* m_pModelExecution;

  ou::tf::PortfolioManager& m_PortfolioManager;
  ou::tf::OrderManager& m_OrderManager;

};

} // namespace tf
} // namespace ou
