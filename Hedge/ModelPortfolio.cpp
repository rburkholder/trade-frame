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

#include <boost/assign/std/vector.hpp>
using namespace boost::assign;

#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/home/phoenix/bind.hpp> 
#include <boost/spirit/home/phoenix/bind/bind_member_function.hpp>


#include "ModelPortfolio.h"

ModelPortfolio::ModelPortfolio(void) 
  : ModelBase(), m_mgrPortfolio( ou::tf::CPortfolioManager::Instance() )
{
  m_vColumnNames += "Name", "Rlzd PL", "Comm.", "Net";
  PopulateWithRootPortfolios();
}

ModelPortfolio::~ModelPortfolio(void) {
}
/*
namespace ProcessPortolios {

  template<class F>
  struct structProcessPortfolioIds {
    structProcessPortfolioIds( ItemPortfolio& parent,
      ModelPortfolio& model, CPortfolioManager& manager )
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
    CPortfolioManager& m_manager;
  };

} // ns ProcessPortfolios
*/
void ModelPortfolio::PopulateWithRootPortfolios( void ) { 
  m_mgrPortfolio.IteratePortfolios( boost::phoenix::bind( &ModelPortfolio::ProcessPortfolioIds, this, boost::phoenix::arg_names::arg1 ) );
}

void ModelPortfolio::ProcessPortfolioIds( const idPortfolio_t& idPortfolio ) {
  mapItems_iter_t iter = m_mapItems.find( idPortfolio );
  if ( m_mapItems.end() == iter ) {
    ItemPortfolio item( m_mgrPortfolio.GetPortfolio( idPortfolio ) );
    iter = m_mapItems.insert( m_mapItems.begin(), mapItem_pair_t( idPortfolio, item ) );
  }
  //ItemPortfolio item( m_manager.GetPortfolio( id ) );
  //m_model.ItemAdded( parent, 
  wxAny anyId = idPortfolio;
  // may desire to use boost::fusion to work on variable types
  // assign a wxDataViewItem
  // process fields to 
}

void ModelPortfolio::ProcessUpdatedItemDetails( ItemPortfolio& item ) {
}