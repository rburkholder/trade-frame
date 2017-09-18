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

#include "stdafx.h"

#include <boost/assign/std/vector.hpp>
using namespace boost::assign;

#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/phoenix/bind.hpp> 
#include <boost/phoenix/bind/bind_member_function.hpp>

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
  setItems_citer_t iter = m_setItems.find( pItem );
  if ( m_setItems.end() == iter ) {
//    DataViewItemPortfolio item( m_mgrPortfolio.GetPortfolio( idPortfolio ) );
//    iter = m_mapItems.insert( m_mapItems.begin(), mapItems_t::value_type( idPortfolio, item ) );
//    iter = m_mapItems.insert( m_mapItems.begin(), mapItems_t::value_type( pItem->GetID(), pItem ) );
    auto pair = m_setItems.insert( setItems_t::value_type( pItem ) );
    ItemChanged( wxDataViewItem( pItem ) );
    //ItemAdded( wxDataViewItem( nullptr ), wxDataViewItem( pItem ) );
  }
  // may desire to use boost::fusion to work on variable types
}

void ModelPortfolio::ProcessUpdatedItemDetails( DataViewItemPortfolio& item ) {
}

bool ModelPortfolio::IsContainer( const wxDataViewItem& item ) const {
  if ( 0 != item.GetID() ) {
    return false;
  }
  else {
    return 0 != m_setItems.size();
  }
}

wxDataViewItem ModelPortfolio::GetParent( const wxDataViewItem& item ) const {
  return wxDataViewItem( nullptr );
}

unsigned int ModelPortfolio::GetChildren(	const wxDataViewItem& item, wxDataViewItemArray& children	) const {

  // need to validate that this is a Portfolio item somehow, because other stuff is stripped away
  // may need refinement to make use of item properly in a tree environment
  unsigned int count = 0;
  if ( 0 == item.GetID() ) {
    for ( setItems_citer_t iter = m_setItems.begin(); m_setItems.end() != iter; ++iter ) {
      children.Add( wxDataViewItem( *iter ) );
    }
    count = m_setItems.size();
  }
  return count;
}

void ModelPortfolio::GetValue( wxVariant& variant, const wxDataViewItem& item, unsigned int col	) const {
  // use fusion to create array of type calls?
  setItems_citer_t iter = m_setItems.find( reinterpret_cast<DataViewItemPortfolio*>( item.GetID() ) );
  assert( m_setItems.end() != iter );
  switch ( col ) {
  case 0:
    (*iter)->AssignFirstColumn( variant );
    break;
  case 1:
    variant = (*iter)->GetPtr()->GetRow().dblRealizedPL;
    break;
  case 2:
    variant = (*iter)->GetPtr()->GetRow().dblCommissionsPaid;
    break;
  case 3: {
    const ou::tf::Portfolio::TableRowDef& row( (*iter)->GetPtr()->GetRow() );
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
  for ( setItems_t::iterator iter = m_setItems.begin(); m_setItems.end() != iter; ++iter ) {
    ItemDeleted( wxDataViewItem( nullptr ), wxDataViewItem( *iter ) );
  }
  m_setItems.clear();
}

} // namespace tf
} // namespace ou
