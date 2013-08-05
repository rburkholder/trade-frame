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

class ModelPortfolioPositionOrderExecution: public ModelBase<ModelPortfolioPositionOrderExecution> { // model for tree of portfolio, position, order, execution
public:

  ModelPortfolioPositionOrderExecution(void);
  ~ModelPortfolioPositionOrderExecution(void);

  void LoadMasterPortfolio( void );

  ModelPortfolio* GetModelPortfolio( void ) { return m_pModelPortfolio; };
  ModelPosition* GetModelPosition( void ) { return m_pModelPosition; };
  ModelOrder* GetModelOrder( void ) { return m_pModelOrder; };
  ModelExecution* GetModelExecution( void ) { return m_pModelExecution; };

  bool IsContainer(	const wxDataViewItem&	item ) const;
  wxDataViewItem GetParent( const wxDataViewItem&	item ) const;
  unsigned int GetChildren(	const wxDataViewItem& item, wxDataViewItemArray& children	) const;
  void GetValue( wxVariant& variant, const wxDataViewItem& item, unsigned int col	) const;

  void ClickedOnTreeItem( void* pItem );

protected:
private:

  typedef Portfolio::idPortfolio_t idPortfolio_t;

  typedef ModelPortfolio::DataViewItemPortfolio DataViewItemPortfolio;
  typedef ModelPosition::DataViewItemPosition DataViewItemPosition;
  typedef ModelOrder::DataViewItemOrder DataViewItemOrder;
  typedef ModelExecution::DataViewItemExecution DataViewItemExecution;

  typedef std::map<void*,DataViewItemBase*> mapItems_t;

//  typedef std::map<void*,DataViewItemPortfolio*> mapItemsPortfolio_t;
//  typedef std::map<void*,DataViewItemPosition*> mapItemsPosition_t;
//  typedef std::map<void*,DataViewItemOrder*> mapItemsOrder_t;
//  typedef std::map<void*,DataViewItemExecution*> mapItemsExecution_t;

  typedef ModelPortfolio::mapItems_t mapItemsPortfolio_t;
  typedef ModelPosition::mapItems_t mapItemsPosition_t;
  typedef ModelOrder::mapItems_t mapItemsOrder_t;
  typedef ModelExecution::mapItems_t mapItemsExecution_t;

  struct ItemExecution: public DataViewItemExecution {  // not in tree, is place holder of items
    ItemExecution( DataViewItemExecution::shared_ptr ptr ): DataViewItemExecution( ptr ) { };
  };
  struct ItemOrder: public DataViewItemOrder {
    ItemOrder( DataViewItemOrder::shared_ptr ptr ): DataViewItemOrder( ptr ) { };
    mapItemsExecution_t mapItemExecution;
    virtual bool IsContainer( void ) { return ( 0 != mapItemExecution.size() ); };
  };
  struct ItemPosition: public DataViewItemPosition {
    ItemPosition( DataViewItemPosition::shared_ptr ptr ): DataViewItemPosition( ptr ) { };
    mapItemsOrder_t mapItemOrder;
    virtual bool IsContainer( void ) { return ( 0 != mapItemOrder.size() ); };
};
  struct ItemPortfolio: public DataViewItemPortfolio {
    ItemPortfolio( DataViewItemPortfolio::shared_ptr ptr ): DataViewItemPortfolio( ptr ) { };
    mapItemsPortfolio_t mapItemPortfolio;
    mapItemsPosition_t mapItemPosition;
    virtual bool IsContainer( void ) { 
      return ( ( 0 != mapItemPortfolio.size() ) || ( 0 != mapItemPosition.size() ) ); };
  };
  struct ItemPortfolioCurrencySummary: public DataViewItemPortfolio {
    ItemPortfolioCurrencySummary( DataViewItemPortfolio::shared_ptr ptr ): DataViewItemPortfolio( ptr ) { ixTypes = ePortfolioCurrency; };
    mapItemsPortfolio_t mapItemPortfolio;
    mapItemsPosition_t mapItemPosition;
    virtual bool IsContainer( void ) { 
      return ( ( 0 != mapItemPortfolio.size() ) || ( 0 != mapItemPosition.size() ) ); };
  };
  struct ItemPortfolioMaster: public DataViewItemPortfolio {
    ItemPortfolioMaster( DataViewItemPortfolio::shared_ptr ptr ): DataViewItemPortfolio( ptr ) { ixTypes = ePortfolioMaster; };
    mapItemsPortfolio_t mapItemPortfolioCurrencySummary;
    virtual bool IsContainer( void ) { return ( 0 != mapItemPortfolioCurrencySummary.size() ); };
  };

  ItemPortfolioMaster* m_pItemPortfolioMaster;
  mapItems_t m_mapItems;  // collection of all items in tree

  ModelPortfolio* m_pModelPortfolio;
  ModelPosition* m_pModelPosition;
  ModelOrder* m_pModelOrder;
  ModelExecution* m_pModelExecution;

  ou::tf::PortfolioManager& m_PortfolioManager;
  ou::tf::OrderManager& m_OrderManager;

  void HandleLoadMasterPortfolio( const idPortfolio_t& idPortfolio );

};

} // namespace tf
} // namespace ou
