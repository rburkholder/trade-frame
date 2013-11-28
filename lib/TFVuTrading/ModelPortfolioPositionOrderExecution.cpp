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

//  m_PortfolioManager.OnPortfolioLoaded.Add( MakeDelegate( this, &ModelPortfolioPositionOrderExecution::HandleOnPortfolioAdded ) );
//  m_PortfolioManager.OnPortfolioAdded.Add( MakeDelegate( this, &ModelPortfolioPositionOrderExecution::HandleOnPortfolioAdded ) );
//  m_PortfolioManager.OnPortfolioUpdated.Add( MakeDelegate( this, &ModelPortfolioPositionOrderExecution::HandleOnPortfolioUpdated ) );
//  m_PortfolioManager.OnPortfolioDeleted.Add( MakeDelegate( this, &ModelPortfolioPositionOrderExecution::HandleOnPortfolioDeleted ) );

//  m_PortfolioManager.OnPositionLoaded.Add( MakeDelegate( this, &ModelPortfolioPositionOrderExecution::HandleOnPositionAdded ) );
//  m_PortfolioManager.OnPositionAdded.Add( MakeDelegate( this, &ModelPortfolioPositionOrderExecution::HandleOnPositionAdded ) );
//  m_PortfolioManager.OnPositionUpdated.Add( MakeDelegate( this, &ModelPortfolioPositionOrderExecution::HandleOnPositionUpdated ) );
//  m_PortfolioManager.OnPositionDeleted.Add( MakeDelegate( this, &ModelPortfolioPositionOrderExecution::HandleOnPositionDeleted ) );

}

ModelPortfolioPositionOrderExecution::~ModelPortfolioPositionOrderExecution(void) {

//  m_PortfolioManager.OnPortfolioLoaded.Remove( MakeDelegate( this, &ModelPortfolioPositionOrderExecution::HandleOnPortfolioAdded ) );
//  m_PortfolioManager.OnPortfolioAdded.Remove( MakeDelegate( this, &ModelPortfolioPositionOrderExecution::HandleOnPortfolioAdded ) );
//  m_PortfolioManager.OnPortfolioUpdated.Remove( MakeDelegate( this, &ModelPortfolioPositionOrderExecution::HandleOnPortfolioUpdated ) );
//  m_PortfolioManager.OnPortfolioDeleted.Remove( MakeDelegate( this, &ModelPortfolioPositionOrderExecution::HandleOnPortfolioDeleted ) );

//  m_PortfolioManager.OnPositionLoaded.Remove( MakeDelegate( this, &ModelPortfolioPositionOrderExecution::HandleOnPositionAdded ) );
//  m_PortfolioManager.OnPositionAdded.Remove( MakeDelegate( this, &ModelPortfolioPositionOrderExecution::HandleOnPositionAdded ) );
//  m_PortfolioManager.OnPositionUpdated.Remove( MakeDelegate( this, &ModelPortfolioPositionOrderExecution::HandleOnPositionUpdated ) );
//  m_PortfolioManager.OnPositionDeleted.Remove( MakeDelegate( this, &ModelPortfolioPositionOrderExecution::HandleOnPositionDeleted ) );

}

// this doesn't work as there is no arg1
void ModelPortfolioPositionOrderExecution::LoadMasterPortfolio( void ) {
  // load the portfolio with "" as id
//  ItemChanged( m_itemNull );  // if this works, then the scan can happen during the resulting event of GetChildren/GetValue
//  m_PortfolioManager.ScanPortfolios( 
//    boost::phoenix::bind( &ModelPortfolio::AddPortfolioToModel, m_pModelPortfolio, boost::phoenix::arg_names::arg1 ) );
//  Cleared();
//  m_PortfolioManager.ScanPortfolios( 
//    boost::phoenix::bind( &ModelPortfolioPositionOrderExecution::HandleLoadMasterPortfolio, this, boost::phoenix::arg_names::arg1 ) );
}

void ModelPortfolioPositionOrderExecution::HandleOnPortfolioAdded( const idPortfolio_t& idPortfolio ) {
  // need the master portofolio first
  // need currency masters next
  // basically need portfolios in hierarchical order
  // or stick portfolios in stack, and scan stack each time new portfolio arrives.  by time all portfolios arrived, stack should be clean
  pPortfolio_t pPortfolio = m_PortfolioManager.GetPortfolio( idPortfolio );
  switch ( pPortfolio->GetRow().ePortfolioType ) {
  case Portfolio::Master: {
    m_pItemPortfolioMaster = new ItemPortfolioMaster( pPortfolio );
    m_pItemPortfolioMaster->pParent = &m_itemNull;
    m_mapItems.insert( mapItems_t::value_type( m_pItemPortfolioMaster->GetID(), m_pItemPortfolioMaster ) );
    ItemAdded( m_itemNull, *m_pItemPortfolioMaster );
//    ItemChanged( *m_pItemPortfolioMaster );
    m_mapPortfolios.insert( mapPortfolios_t::value_type( idPortfolio, m_pItemPortfolioMaster ) );
                          }
    break;
  case Portfolio::CurrencySummary: {
    ItemPortfolioCurrencySummary* p( new ItemPortfolioCurrencySummary( pPortfolio ) );
    m_mapItems.insert( mapItems_t::value_type( p->GetID(), p ) );
//    ItemAdded( m_itemNull, *p );
//    ItemChanged( *p );
    m_mapPortfolios.insert( mapPortfolios_t::value_type( idPortfolio, p ) );
    m_mapUnattachedTreeItems.insert( mapUnattachedTreeItems_t::value_type( p->GetID(), p ) );
    BuildTreeFromUnattachedTreeItems();
                                   }
    break;
  case Portfolio::Standard:
  case Portfolio::Basket:
  case Portfolio::MultiLeggedPosition: {
    ItemPortfolio* p( new ItemPortfolio( pPortfolio ) );
    m_mapItems.insert( mapItems_t::value_type( p->GetID(), p ) );
//    ItemAdded( m_itemNull, *p );
//    ItemChanged( *p );
    m_mapPortfolios.insert( mapPortfolios_t::value_type( idPortfolio, p ) );
    m_mapUnattachedTreeItems.insert( mapUnattachedTreeItems_t::value_type( p->GetID(), p ) );
    BuildTreeFromUnattachedTreeItems();
                                       }
    break;
  default:  throw std::runtime_error( "HandleOnPortfolioAdded doesn't know new Portfolio type" );
  }
}

void ModelPortfolioPositionOrderExecution::BuildTreeFromUnattachedTreeItems( void ) {
  bool bTreeUpdated;
  do {
    bTreeUpdated = false;
    for ( mapUnattachedTreeItems_t::iterator iter1 = m_mapUnattachedTreeItems.begin(); m_mapUnattachedTreeItems.end() != iter1; ++iter1 ) {
      idPortfolio_t idOwner = iter1->second->Value()->GetRow().idOwner;
      assert( "" != idOwner );
      mapPortfolios_t::iterator iter2 = m_mapPortfolios.find( idOwner );
      if ( m_mapPortfolios.end() != iter2 ) {
        switch ( iter2->second->ixType ) {
        case ePortfolioMaster:
          iter1->second->pParent = iter2->second;
          dynamic_cast<ItemPortfolioMaster*>( iter2->second )->mapItemPortfolioCurrencySummary.insert( mapItemsPortfolio_t::value_type( iter1->first, iter1->second ) );
          ItemAdded( *iter2->second, *iter1->second );
          break;
        case ePortfolioCurrency:
          iter1->second->pParent = iter2->second;
          dynamic_cast<ItemPortfolioCurrencySummary*>( iter2->second )->mapItemPortfolio.insert( mapItemsPortfolio_t::value_type( iter1->first, iter1->second ) );
          ItemAdded( *iter2->second, *iter1->second );
          break;
        case ePortfolio:
          iter1->second->pParent = iter2->second;
          dynamic_cast<ItemPortfolio*>( iter2->second )->mapItemPortfolio.insert( mapItemsPortfolio_t::value_type( iter1->first, iter1->second ) );
          ItemAdded( *iter2->second, *iter1->second );
          break;
        default: throw std::runtime_error( "BuildTreeFromUnattachedTreeItems has no enumeration" );
        }
        m_mapUnattachedTreeItems.erase( iter1 );
        bTreeUpdated = true;
        break;
      }
    }
  } while ( bTreeUpdated );
}

void ModelPortfolioPositionOrderExecution::HandleOnPortfolioUpdated( const idPortfolio_t& ) {
}

void ModelPortfolioPositionOrderExecution::HandleOnPortfolioDeleted( const idPortfolio_t& ) {
}

void ModelPortfolioPositionOrderExecution::HandleOnPositionAdded( const idPosition_t& ) {
}

void ModelPortfolioPositionOrderExecution::HandleOnPositionUpdated( const idPosition_t& ) {
}

void ModelPortfolioPositionOrderExecution::HandleOnPositionDeleted( const idPosition_t& ) {
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
    mapItems_t::const_iterator iter = m_mapItems.find( item.GetID() );
    assert( m_mapItems.end() != iter );
    switch (iter->second->ixType) {
    case ePortfolioMaster: {
      mapItemsPortfolio_t& map( dynamic_cast<ItemPortfolioMaster*>( iter->second )->mapItemPortfolioCurrencySummary );
      for ( mapItemsPortfolio_t::const_iterator iter = map.begin(); map.end() != iter; ++iter ) {
        children.Add( *iter->second );
        ++count;
      }
                           }
      break;
    case ePortfolioCurrency: {
      mapItemsPortfolio_t& map1( dynamic_cast<ItemPortfolioCurrencySummary*>( iter->second )->mapItemPortfolio );
      for ( mapItemsPortfolio_t::const_iterator iter = map1.begin(); map1.end() != iter; ++iter ) {
        children.Add( *iter->second );
        ++count;
      }
      mapItemsPosition_t& map2( dynamic_cast<ItemPortfolioCurrencySummary*>( iter->second )->mapItemPosition );
      for ( mapItemsPosition_t::const_iterator iter = map2.begin(); map2.end() != iter; ++iter ) {
        children.Add( *iter->second );
        ++count;
      }
                           }
      break;
    case ePortfolio: {
      mapItemsPortfolio_t& map1( dynamic_cast<ItemPortfolioCurrencySummary*>( iter->second )->mapItemPortfolio );
      for ( mapItemsPortfolio_t::const_iterator iter = map1.begin(); map1.end() != iter; ++iter ) {
        children.Add( *iter->second );
        ++count;
      }
      mapItemsPosition_t& map2( dynamic_cast<ItemPortfolioCurrencySummary*>( iter->second )->mapItemPosition );
      for ( mapItemsPosition_t::const_iterator iter = map2.begin(); map2.end() != iter; ++iter ) {
        children.Add( *iter->second );
        ++count;
      }
                           }
      break;
    case ePosition: {
      mapItemsOrder_t& map1( dynamic_cast<ItemPosition*>( iter->second )->mapItemOrder );
      for ( mapItemsOrder_t::const_iterator iter = map1.begin(); map1.end() != iter; ++iter ) {
        children.Add( *iter->second );
        ++count;
      }
                    }
      break;
    case eOrder: {
      mapItemsExecution_t& map1( dynamic_cast<ItemOrder*>( iter->second )->mapItemExecution );
      for ( mapItemsExecution_t::const_iterator iter = map1.begin(); map1.end() != iter; ++iter ) {
        children.Add( *iter->second );
        ++count;
      }
                    }
      break;
    case eExecution:
      throw std::runtime_error( "eExecution found, shouldn't be" );
      break;
    }
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
  switch (iter->second->ixType) {
    case ePortfolioMaster:
      m_pModelExecution->ClearItems();
      m_pModelOrder->ClearItems();
      m_pModelPosition->ClearItems();
      m_pModelPortfolio->ClearItems();
      m_pModelPortfolio->AddPortfolioToModel( dynamic_cast<DataViewItemPortfolio*>( iter->second ) );
      // add the currency summary portfolios as well?
      break;
    case ePortfolioCurrency:
      m_pModelExecution->ClearItems();
      m_pModelOrder->ClearItems();
      m_pModelPosition->ClearItems();
      m_pModelPortfolio->AddPortfolioToModel( dynamic_cast<DataViewItemPortfolio*>( iter->second ) );
      // show all sub portfolios and positions?  or just in tree?
      break;
    case ePortfolio:
      m_pModelExecution->ClearItems();
      m_pModelOrder->ClearItems();
      m_pModelPosition->ClearItems();
      m_pModelPortfolio->ClearItems();
      m_pModelPortfolio->AddPortfolioToModel( dynamic_cast<DataViewItemPortfolio*>( iter->second ) );
      // show sub portfolios and positions?  how to show sub portfolios as members?
      break;
    case ePosition:
      m_pModelExecution->ClearItems();
      m_pModelOrder->ClearItems();
      m_pModelPosition->ClearItems();
      // show orders?
      break;
    case eOrder:
      m_pModelExecution->ClearItems();
      m_pModelOrder->ClearItems();
      // show executions?
      break;
    case eExecution:
      m_pModelExecution->ClearItems();
      // not sure if we get here or not
      break;
    case eUnknown:
      assert( 0 );
      break;
  }
}

EModelType ModelPortfolioPositionOrderExecution::GetModelType( const wxDataViewItem& item ) {
  // replace other scattered code with a call to this method
  mapItems_t::const_iterator iter = m_mapItems.find( item.GetID() );
  assert( iter != m_mapItems.end() );
  return (iter->second->ixType);
}

} // namespace tf
} // namespace ou
