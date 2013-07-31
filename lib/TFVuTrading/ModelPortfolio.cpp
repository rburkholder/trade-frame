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

#include "StdAfx.h"

#include <boost/assign/std/vector.hpp>
using namespace boost::assign;

#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/home/phoenix/bind.hpp> 
#include <boost/spirit/home/phoenix/bind/bind_member_function.hpp>

#include "ModelPortfolio.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

ModelPortfolio::ModelPortfolio(void) 
  : ModelBase(), m_mgrPortfolio( ou::tf::PortfolioManager::Instance() )
{
  m_vColumnNames += "Name", "Rlzd PL", "Comm.", "Net";
  m_mgrPortfolio.OnPortfolioAdded.Add( MakeDelegate( this, &ModelPortfolio::AddPortfolioToModel ) );
//  PopulateWithRootPortfolios();
}

ModelPortfolio::~ModelPortfolio(void) {
  m_mgrPortfolio.OnPortfolioAdded.Remove( MakeDelegate( this, &ModelPortfolio::AddPortfolioToModel ) );
}
/*
// may not need this any more.  old style of processing?
namespace ProcessPortolios {

  template<class F>
  struct structProcessPortfolioIds {
    structProcessPortfolioIds( ItemPortfolio& parent,
      ModelPortfolio& model, PortfolioManager& manager )
      : m_parent( parent ), m_model( model ), m_manager( manager ) {};
    void operator()( ou::tf::keytypes::idPortfolio_t& id ) {
      mapItems_iter_t iter = m_mapItems.find( id );
      if ( m_mapItems.end() == iter ) {
      }
      ItemPortfolio item( m_manager.GetPortfolio( id ) );
      //m_model.ItemAdded( parent, 
      wxAny anyId = id;
      // may desire to use boost::fusion to work on variable types
      // assign a wxDataViewItem
      // process fields to 
    };
  private:
    ItemPortfolio& m_parent;
    ModelPortfolio& m_model;
    PortfolioManager& m_manager;
  };

} // ns ProcessPortfolios
*/
void ModelPortfolio::PopulateWithRootPortfolios( void ) { 
  m_mgrPortfolio.ScanPortfolios( boost::phoenix::bind( &ModelPortfolio::AddPortfolioToModel, this, boost::phoenix::arg_names::arg1 ) );
}

// part of the process of the initial sync, can be used for adding additional portfolios
void ModelPortfolio::AddPortfolioToModel( const idPortfolio_t& idPortfolio ) {
  mapItems_iter_t iter = m_mapItems.find( idPortfolio );
  if ( m_mapItems.end() == iter ) {
    ItemPortfolio item( m_mgrPortfolio.GetPortfolio( idPortfolio ) );
    iter = m_mapItems.insert( m_mapItems.begin(), mapItem_pair_t( idPortfolio, item ) );
    ItemAdded( itemNull, item );
  }
  // may desire to use boost::fusion to work on variable types
}

void ModelPortfolio::ProcessUpdatedItemDetails( ItemPortfolio& item ) {
}

} // namespace tf
} // namespace ou
