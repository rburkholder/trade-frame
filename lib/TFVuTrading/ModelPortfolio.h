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

#include <map>

#include <TFTrading/PortfolioManager.h>

#include "ModelBase.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

class ModelPortfolio: public ModelBase {
public:

  ModelPortfolio(void);
  ~ModelPortfolio(void);

protected:
private:

  typedef ou::tf::PortfolioManager PortfolioManager;
  typedef PortfolioManager::pPortfolio_t pPortfolio_t;
  typedef PortfolioManager::idPortfolio_t idPortfolio_t;

  struct ItemPortfolio: public wxDataViewItem {
    ItemPortfolio( pPortfolio_t& pPortfolio )
    : m_pPortfolio( pPortfolio ), 
      wxDataViewItem( reinterpret_cast<void*>( 1 ) ) {  };
    ItemPortfolio( const ItemPortfolio& item ) 
      : m_pPortfolio( item.m_pPortfolio ), wxDataViewItem( *this ) {};
    ~ItemPortfolio( void ) {  };
    pPortfolio_t m_pPortfolio;
  };

  typedef std::pair<idPortfolio_t, ItemPortfolio> mapItem_pair_t;
  typedef std::map<idPortfolio_t, ItemPortfolio> mapItems_t;
  typedef mapItems_t::const_iterator mapItems_iter_t;
  mapItems_t m_mapItems;

  PortfolioManager& m_mgrPortfolio;  // database must be open before processing portfolios

  void PopulateWithRootPortfolios( void );

  void AddPortfolioToModel( const idPortfolio_t& idPortfolio );
  void ProcessUpdatedItemDetails( ItemPortfolio& item );

};

} // namespace tf
} // namespace ou
