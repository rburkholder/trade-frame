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

#include <map>

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

  bool IsContainer(	const wxDataViewItem&	item ) const { return true; };  // true for being tree

  void LoadMasterPortfolio( void );

  ModelPortfolio* GetModelPortfolio( void ) { return m_pModelPortfolio; };
  ModelPosition* GetModelPosition( void ) { return m_pModelPosition; };
  ModelOrder* GetModelOrder( void ) { return m_pModelOrder; };
  ModelExecution* GetModelExecution( void ) { return m_pModelExecution; };

  typedef ModelPortfolio::DataViewItemPortfolio DataViewItemPortfolio;
  typedef ModelPosition::DataViewItemPosition DataViewItemPosition;
  typedef ModelOrder::DataViewItemOrder DataViewItemOrder;
  typedef ModelExecution::DataViewItemExecution DataViewItemExecution;

  unsigned int GetChildren(	const wxDataViewItem& item, wxDataViewItemArray& children	) const;
  void GetValue( wxVariant& variant, const wxDataViewItem& item, unsigned int col	) const;

  struct ItemExecution: public DataViewItemExecution {  // not in tree, is place holder of items
  };
  struct ItemOrder: public DataViewItemOrder {
    std::set<ItemExecution> setItemExecution;
  };
  struct ItemPosition: public DataViewItemPosition {
    std::set<ItemOrder> setItemOrder;
  };
  struct ItemPortfolio: public DataViewItemPortfolio {
    std::set<ItemPortfolio> setItemPortfolio;
    std::set<ItemPosition> setItemPosition;
  };
  struct ItemPortfolioCurrencySummary: public DataViewItemPortfolio {
    std::set<ItemPortfolio> setItemPortfolio;
    std::set<ItemPosition> setItemPosition;
  };
  struct ItemPortfolioMaster: public DataViewItemPortfolio {
    ItemPortfolioMaster( DataViewItemPortfolio::shared_ptr ptr ): DataViewItemPortfolio( ptr ) {};
    std::set<ItemPortfolio> ItemPortfolioCurrencySummary;
  };

protected:
private:

  typedef Portfolio::idPortfolio_t idPortfolio_t;

  ModelPortfolio* m_pModelPortfolio;
  ModelPosition* m_pModelPosition;
  ModelOrder* m_pModelOrder;
  ModelExecution* m_pModelExecution;

  ou::tf::PortfolioManager& m_PortfolioManager;
  ou::tf::OrderManager& m_OrderManager;

  typedef std::map<void*,ModelBase::DataViewItemBase*> mapItems_t;
  mapItems_t m_mapItems;
  ItemPortfolioMaster* m_pItemPortfolioMaster;

  void HandleLoadMasterPortfolio( const idPortfolio_t& idPortfolio );

};

} // namespace tf
} // namespace ou
