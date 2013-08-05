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

#include "StdAfx.h"

#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/home/phoenix/bind.hpp> 
#include <boost/spirit/home/phoenix/bind/bind_member_function.hpp>

#include <boost/assign/std/vector.hpp>
using namespace boost::assign;

#include "ModelPortfolioPositionOrderExecution.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

ModelPortfolioPositionOrderExecution::ModelPortfolioPositionOrderExecution(void) 
  : ModelBase<ModelPortfolioPositionOrderExecution>(), 
  m_PortfolioManager( ou::tf::PortfolioManager::Instance() ), m_OrderManager( ou::tf::OrderManager::Instance() ),
  m_pItemPortfolioMaster( 0 )
{

  m_vColumnNames += "Tree";

  m_pModelPortfolio = new ModelPortfolio;
  m_pModelPosition = new ModelPosition;
  m_pModelOrder = new ModelOrder;
  m_pModelExecution = new ModelExecution;

  m_PortfolioManager.LoadActivePortfolios();
}

ModelPortfolioPositionOrderExecution::~ModelPortfolioPositionOrderExecution(void) {
//  delete m_pModelPortfolio;
//  delete m_pModelPosition;
//  delete m_pModelOrder;
//  delete m_pModelExecution;
}

void ModelPortfolioPositionOrderExecution::LoadMasterPortfolio( void ) {
  // load the portfolio with "" as id
//  ItemChanged( m_itemNull );  // if this works, then the scan can happen during the resulting event of GetChildren/GetValue
//  m_PortfolioManager.ScanPortfolios( 
//    boost::phoenix::bind( &ModelPortfolio::AddPortfolioToModel, m_pModelPortfolio, boost::phoenix::arg_names::arg1 ) );
//  Cleared();
  m_PortfolioManager.ScanPortfolios( 
    boost::phoenix::bind( &ModelPortfolioPositionOrderExecution::HandleLoadMasterPortfolio, this, boost::phoenix::arg_names::arg1 ) );
}

void ModelPortfolioPositionOrderExecution::HandleLoadMasterPortfolio( const idPortfolio_t& idPortfolio ) {
  m_pItemPortfolioMaster = new ItemPortfolioMaster( m_PortfolioManager.GetPortfolio( idPortfolio ) );
  m_mapItems.insert( mapItems_t::value_type( m_pItemPortfolioMaster->GetID(), m_pItemPortfolioMaster ) );
  ItemAdded( m_itemNull, *m_pItemPortfolioMaster );
  ItemChanged( *m_pItemPortfolioMaster );
}

bool ModelPortfolioPositionOrderExecution::IsContainer(	const wxDataViewItem&	item ) const {
  bool bReturn = false;
  if ( 0 == item.GetID() ) {
    bReturn = true;
  }
  else {
    mapItems_t::const_iterator iter = m_mapItems.find( item.GetID() );
    assert( m_mapItems.end() != iter );
    bReturn = iter->second->IsContainer();
  }
  return bReturn;
}

wxDataViewItem ModelPortfolioPositionOrderExecution::GetParent( const wxDataViewItem&	item ) const {
  if ( 0 == item.GetID() ) {
    assert( 0 );
  }
  mapItems_t::const_iterator iter = m_mapItems.find( item.GetID() );
  assert( m_mapItems.end() != iter );
  return ( *iter->second->pParent );
}

unsigned int ModelPortfolioPositionOrderExecution::GetChildren(	const wxDataViewItem& item, wxDataViewItemArray& children	) const {
  // with columns this gets called without having to scan
  unsigned int count( 0 );
  if ( 0 == item.GetID() ) {
    if ( 0 != m_pItemPortfolioMaster ) {
      children.Add( *m_pItemPortfolioMaster );
      count = 1;
    }
  }
  else {
  }
  return count;
}

void ModelPortfolioPositionOrderExecution::GetValue( wxVariant& variant, const wxDataViewItem& item, unsigned int col	) const {
  mapItems_t::const_iterator iter = m_mapItems.find( item.GetID() );
  assert( iter != m_mapItems.end() );
  iter->second->AssignFirstColumn( variant );
}

void ModelPortfolioPositionOrderExecution::ClickedOnTreeItem( void* pItem ) {
  mapItems_t::const_iterator iter = m_mapItems.find( pItem );
  assert( iter != m_mapItems.end() );
  switch (iter->second->ixTypes) {
    case ePortfolioMaster:
      m_pModelExecution->ClearItems();
      m_pModelOrder->ClearItems();
      m_pModelPosition->ClearItems();
      m_pModelPortfolio->ClearItems();
      m_pModelPortfolio->AddPortfolioToModel( dynamic_cast<DataViewItemPortfolio*>( iter->second ) );
      break;
    case ePortfolioCurrency:
      break;
    case ePortfolio:
      break;
    case ePosition:
      break;
    case eOrder:
      break;
    case eExecution:
      break;
    case eUnknown:
      assert( 0 );
      break;
  }
}

} // namespace tf
} // namespace ou
