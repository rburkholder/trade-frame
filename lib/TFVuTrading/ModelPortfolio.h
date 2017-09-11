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
#include <set>

#include <TFTrading/PortfolioManager.h>

#include "ModelBase.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

class ModelPortfolio: public ModelBase<ModelPortfolio> {
public:

  typedef ou::tf::PortfolioManager PortfolioManager;
  typedef PortfolioManager::idPortfolio_t idPortfolio_t;
  typedef PortfolioManager::pPortfolio_t pPortfolio_t;

  struct DataViewItemPortfolio: public DataViewItem<pPortfolio_t::element_type> {
    DataViewItemPortfolio( shared_ptr& ptr )
      : DataViewItem<pPortfolio_t::element_type>( ptr ) { ixType = ePortfolio; };
    void AssignFirstColumn( wxVariant& variant ) const {
      variant = m_ptr->GetRow().idPortfolio;
    }
  };

  typedef std::map<void*,DataViewItemPortfolio*> mapItems_t;

  ModelPortfolio(void);
  ~ModelPortfolio(void);

  //void AddPortfolioToModel( const idPortfolio_t& idPortfolio );
  void AddPortfolioToModel( DataViewItemPortfolio* );
  void ClearItems( void );

protected:
private:

//  typedef std::map<idPortfolio_t, DataViewItemPortfolio> mapItems_t;
  typedef mapItems_t::const_iterator mapItems_citer_t;
  mapItems_t m_mapItems;

  unsigned int GetChildren( const wxDataViewItem& item, wxDataViewItemArray& children ) const;
  void GetValue( wxVariant& variant, const wxDataViewItem& item, unsigned int col ) const;

  PortfolioManager& m_mgrPortfolio;  // database must be open before processing portfolios

  void ProcessUpdatedItemDetails( DataViewItemPortfolio& item );

};

} // namespace tf
} // namespace ou
