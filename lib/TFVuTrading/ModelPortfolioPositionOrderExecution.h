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
#include <set>

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

  virtual bool IsContainer( const wxDataViewItem& item ) const;
  virtual wxDataViewItem GetParent( const wxDataViewItem& item ) const;
  virtual unsigned int GetChildren( const wxDataViewItem& item, wxDataViewItemArray& children ) const;
  virtual void GetValue( wxVariant& variant, const wxDataViewItem& item, unsigned int col ) const;

  EModelType GetModelType( const wxDataViewItem& item );

  void ClickedOnTreeItem( DataViewItemBase* pItem );

protected:
private:

  typedef Portfolio::idPortfolio_t idPortfolio_t;
  typedef Portfolio::pPortfolio_t pPortfolio_t;

  typedef Position::idPosition_t idPosition_t;
  typedef Position::pPosition_t pPosition_t;

  typedef ModelPortfolio::DataViewItemPortfolio DataViewItemPortfolio;
  typedef ModelPosition::DataViewItemPosition DataViewItemPosition;
  typedef ModelOrder::DataViewItemOrder DataViewItemOrder;
  typedef ModelExecution::DataViewItemExecution DataViewItemExecution;

  //typedef std::map<void*,DataViewItemBase*> mapItems_t;
  typedef std::set<DataViewItemBase*> setItems_t;

  typedef ModelPortfolio::setItems_t setItemsPortfolio_t;
  typedef ModelPosition::setItems_t setItemsPosition_t;
  typedef ModelOrder::setItems_t setItemsOrder_t;
  typedef ModelExecution::setItems_t setItemsExecution_t;

  struct ItemExecution: public DataViewItemExecution {  // not in tree, is place holder of items
    ItemExecution( DataViewItemExecution::shared_ptr ptr ): DataViewItemExecution( ptr ) { };
  };
  struct ItemOrder: public DataViewItemOrder {
    ItemOrder( DataViewItemOrder::shared_ptr ptr ): DataViewItemOrder( ptr ) { };
    setItemsExecution_t setItemExecution;
    virtual bool IsContainer( void ) const { return ( 0 != setItemExecution.size() ); };
  };
  struct ItemPosition: public DataViewItemPosition {
    ItemPosition( DataViewItemPosition::shared_ptr ptr ): DataViewItemPosition( ptr ) { };
    setItemsOrder_t setItemOrder;
    virtual bool IsContainer( void ) const { return ( 0 != setItemOrder.size() ); };
  };
  struct ItemPortfolio: public DataViewItemPortfolio {
    ItemPortfolio( DataViewItemPortfolio::shared_ptr ptr ): DataViewItemPortfolio( ptr ) { };
    setItemsPortfolio_t setItemPortfolio;
    setItemsPosition_t setItemPosition;
    virtual bool IsContainer( void ) const { 
      return ( ( 0 != setItemPortfolio.size() ) || ( 0 != setItemPosition.size() ) ); 
    }
    
  };
  struct ItemPortfolioCurrencySummary: public DataViewItemPortfolio {
    ItemPortfolioCurrencySummary( DataViewItemPortfolio::shared_ptr ptr ): DataViewItemPortfolio( EMTPortfolioCurrency, ptr ) {};
    setItemsPortfolio_t setItemPortfolio;
    setItemsPosition_t  setItemPosition;
    virtual bool IsContainer( void ) const { 
      return ( ( 0 != setItemPortfolio.size() ) || ( 0 != setItemPosition.size() ) ); 
    }
  };
  struct ItemPortfolioMaster: public DataViewItemPortfolio {
    ItemPortfolioMaster( DataViewItemPortfolio::shared_ptr ptr ): DataViewItemPortfolio( EMTPortfolioMaster, ptr ) {};
    setItemsPortfolio_t setItemPortfolioCurrencySummary;
    virtual bool IsContainer( void ) const { 
      return ( 0 != setItemPortfolioCurrencySummary.size() ); 
    }
  };

  ItemPortfolioMaster* m_pItemPortfolioMaster;
  setItems_t m_setItems;  // collection of all items in tree

  ModelPortfolio* m_pModelPortfolio;
  ModelPosition* m_pModelPosition;
  ModelOrder* m_pModelOrder;
  ModelExecution* m_pModelExecution;

  ou::tf::PortfolioManager& m_PortfolioManager;
  ou::tf::OrderManager& m_OrderManager;

  //typedef std::map<void*,DataViewItemPortfolio*> mapUnattachedTreeItems_t;
  typedef std::set<DataViewItemPortfolio*> setUnattachedTreeItems_t;
  //mapUnattachedTreeItems_t m_mapUnattachedTreeItems;
  setUnattachedTreeItems_t m_setUnattachedTreeItems;
  
  void BuildTreeFromUnattachedTreeItems( void );

  typedef std::map<idPortfolio_t,DataViewItemPortfolio*> mapPortfolios_t;
  mapPortfolios_t m_mapPortfolios;

  void HandleOnPortfolioLoaded( pPortfolio_t& ) {};  // 2013/11/09 details to be determined
  void HandleOnPortfolioAdded( pPortfolio_t& );
  void HandleOnPortfolioUpdated( pPortfolio_t& );
  void HandleOnPortfolioDeleted( const idPortfolio_t& );

  void HandleOnPositionLoaded( pPosition_t& ) {};  // 2013/11/09 details to be determined
  void HandleOnPositionAdded( pPosition_t& );
  void HandleOnPositionUpdated( pPosition_t& );
  void HandleOnPositionDeleted( const idPosition_t& );

  void HandleLoadMasterPortfolio( const idPortfolio_t& idPortfolio );

};

} // namespace tf
} // namespace ou
