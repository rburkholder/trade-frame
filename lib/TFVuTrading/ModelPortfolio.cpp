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
  : ModelBase<ModelPortfolio>(), m_mgrPortfolio( ou::tf::PortfolioManager::Instance() )
{
  m_vColumnNames += "Name", "Rlzd PL", "Comm.", "Net";
//  m_mgrPortfolio.OnPortfolioAdded.Add( MakeDelegate( this, &ModelPortfolio::AddPortfolioToModel ) );
//  PopulateWithRootPortfolios();
}

ModelPortfolio::~ModelPortfolio(void) {
//  m_mgrPortfolio.OnPortfolioAdded.Remove( MakeDelegate( this, &ModelPortfolio::AddPortfolioToModel ) );
}

// part of the process of the initial sync, can be used for adding additional portfolios
//void ModelPortfolio::AddPortfolioToModel( const idPortfolio_t& idPortfolio ) {
void ModelPortfolio::AddPortfolioToModel( DataViewItemPortfolio* pItem ) {
  mapItems_citer_t iter = m_mapItems.find( pItem->GetID() );
  if ( m_mapItems.end() == iter ) {
//    DataViewItemPortfolio item( m_mgrPortfolio.GetPortfolio( idPortfolio ) );
//    iter = m_mapItems.insert( m_mapItems.begin(), mapItems_t::value_type( idPortfolio, item ) );
//    iter = m_mapItems.insert( m_mapItems.begin(), mapItems_t::value_type( pItem->GetID(), pItem ) );
    iter = m_mapItems.insert( m_mapItems.begin(), mapItems_t::value_type( pItem->GetID(), pItem ) );
    ItemAdded( m_itemNull, *pItem );
    //ItemChanged( *pItem );
  }
  // may desire to use boost::fusion to work on variable types
}

void ModelPortfolio::ProcessUpdatedItemDetails( DataViewItemPortfolio& item ) {
}

unsigned int ModelPortfolio::GetChildren(	const wxDataViewItem& item, wxDataViewItemArray& children	) const {

  // need to validate that this is a Portfolio item somehow, because other stuff is stripped away
  // may need refinement to make use of item properly in a tree environment
  unsigned int count = 0;
  if ( 0 == item.GetID() ) {
    for ( mapItems_citer_t iter = m_mapItems.begin(); m_mapItems.end() != iter; ++iter ) {
      children.Add( *iter->second );
    }
    count = m_mapItems.size();
  }
  return count;
}

void ModelPortfolio::GetValue( wxVariant& variant, const wxDataViewItem& item, unsigned int col	) const {
  // use fusion to create array of type calls?
  mapItems_citer_t iter = m_mapItems.find( item.GetID() );
  assert( m_mapItems.end() != iter );
  switch ( col ) {
  case 0:
    iter->second->AssignFirstColumn( variant );
    break;
  case 1:
    variant = iter->second->Value()->GetRow().dblRealizedPL;
    break;
  case 2:
    variant = iter->second->Value()->GetRow().dblCommissionsPaid;
    break;
  case 3: {
    const ou::tf::Portfolio::TableRowDef& row( iter->second->Value()->GetRow() );
    variant = row.dblRealizedPL - row.dblCommissionsPaid;
          }
    break;
  default:
    std::cout << "col " << col << std::endl;
  }
}

// maybe add to base class
// need to track point of last click, if last click was same place, don't do the same thing
void ModelPortfolio::ClearItems( void ) {
  for ( mapItems_t::iterator iter = m_mapItems.begin(); m_mapItems.end() != iter; ++iter ) {
    ItemDeleted( m_itemNull, *iter->second );
  }
  m_mapItems.clear();
}

} // namespace tf
} // namespace ou
