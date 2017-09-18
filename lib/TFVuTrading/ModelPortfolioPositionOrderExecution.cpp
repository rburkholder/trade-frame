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

#include "stdafx.h"

#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/phoenix/bind.hpp> 
#include <boost/phoenix/bind/bind_member_function.hpp>

#include <boost/assign/std/vector.hpp>
using namespace boost::assign;

#include "ModelPortfolioPositionOrderExecution.h"

// need to use the following as example:
// https://github.com/wxWidgets/wxWidgets/blob/v3.1.0/samples/dataview/mymodels.h
// todo:  free up childrn?

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

  m_PortfolioManager.OnPortfolioLoaded.Add( MakeDelegate( this, &ModelPortfolioPositionOrderExecution::HandleOnPortfolioAdded ) );
  m_PortfolioManager.OnPortfolioAdded.Add( MakeDelegate( this, &ModelPortfolioPositionOrderExecution::HandleOnPortfolioAdded ) );
  m_PortfolioManager.OnPortfolioUpdated.Add( MakeDelegate( this, &ModelPortfolioPositionOrderExecution::HandleOnPortfolioUpdated ) );
  m_PortfolioManager.OnPortfolioDeleted.Add( MakeDelegate( this, &ModelPortfolioPositionOrderExecution::HandleOnPortfolioDeleted ) );

  m_PortfolioManager.OnPositionLoaded.Add( MakeDelegate( this, &ModelPortfolioPositionOrderExecution::HandleOnPositionAdded ) );
  m_PortfolioManager.OnPositionAdded.Add( MakeDelegate( this, &ModelPortfolioPositionOrderExecution::HandleOnPositionAdded ) );
  m_PortfolioManager.OnPositionUpdated.Add( MakeDelegate( this, &ModelPortfolioPositionOrderExecution::HandleOnPositionUpdated ) );
  m_PortfolioManager.OnPositionDeleted.Add( MakeDelegate( this, &ModelPortfolioPositionOrderExecution::HandleOnPositionDeleted ) );

}

ModelPortfolioPositionOrderExecution::~ModelPortfolioPositionOrderExecution(void) {

  m_PortfolioManager.OnPortfolioLoaded.Remove( MakeDelegate( this, &ModelPortfolioPositionOrderExecution::HandleOnPortfolioAdded ) );
  m_PortfolioManager.OnPortfolioAdded.Remove( MakeDelegate( this, &ModelPortfolioPositionOrderExecution::HandleOnPortfolioAdded ) );
  m_PortfolioManager.OnPortfolioUpdated.Remove( MakeDelegate( this, &ModelPortfolioPositionOrderExecution::HandleOnPortfolioUpdated ) );
  m_PortfolioManager.OnPortfolioDeleted.Remove( MakeDelegate( this, &ModelPortfolioPositionOrderExecution::HandleOnPortfolioDeleted ) );

  m_PortfolioManager.OnPositionLoaded.Remove( MakeDelegate( this, &ModelPortfolioPositionOrderExecution::HandleOnPositionAdded ) );
  m_PortfolioManager.OnPositionAdded.Remove( MakeDelegate( this, &ModelPortfolioPositionOrderExecution::HandleOnPositionAdded ) );
  m_PortfolioManager.OnPositionUpdated.Remove( MakeDelegate( this, &ModelPortfolioPositionOrderExecution::HandleOnPositionUpdated ) );
  m_PortfolioManager.OnPositionDeleted.Remove( MakeDelegate( this, &ModelPortfolioPositionOrderExecution::HandleOnPositionDeleted ) );

}

void ModelPortfolioPositionOrderExecution::LoadMasterPortfolio( void ) {
  // load the portfolio with "" as id
//  ItemChanged( m_itemNull );  // if this works, then the scan can happen during the resulting event of GetChildren/GetValue
//  m_PortfolioManager.ScanPortfolios( 
//    boost::phoenix::bind( &ModelPortfolio::AddPortfolioToModel, m_pModelPortfolio, boost::phoenix::arg_names::arg1 ) );
//  Cleared();
  // portfolio may not be loaded yet, as ScanPortfolios works off of the map
  m_PortfolioManager.ScanPortfolios( "master",
    boost::phoenix::bind( &ModelPortfolioPositionOrderExecution::HandleLoadMasterPortfolio, this, boost::phoenix::arg_names::arg1 ) );
}

void ModelPortfolioPositionOrderExecution::HandleOnPortfolioAdded( pPortfolio_t& pPortfolio ) {
  // need the master portfolio first
  // need currency masters next
  // basically need portfolios in hierarchical order
  // or stick portfolios in stack, and scan stack each time new portfolio arrives.  by time all portfolios arrived, stack should be clean
  //pPortfolio_t pPortfolio = m_PortfolioManager.GetPortfolio( idPortfolio );
  idPortfolio_t idPortfolio = pPortfolio->Id();
  switch ( pPortfolio->GetRow().ePortfolioType ) {
  case Portfolio::Master: {
    m_pItemPortfolioMaster = new ItemPortfolioMaster( pPortfolio );
    //m_pItemPortfolioMaster->SetParent( nullptr );
    m_setItems.insert( setItems_t::value_type( m_pItemPortfolioMaster ) );
    m_mapPortfolios.insert( mapPortfolios_t::value_type( idPortfolio, m_pItemPortfolioMaster ) );
    //HandleLoadMasterPortfolio( idPortfolio );
    ItemChanged( wxDataViewItem( m_pItemPortfolioMaster ) );
    ItemAdded( wxDataViewItem( nullptr ), wxDataViewItem( m_pItemPortfolioMaster ) );
    //m_mapUnattachedTreeItems.insert( mapUnattachedTreeItems_t::value_type( p->GetID(), m_pItemPortfolioMaster ) );
    //BuildTreeFromUnattachedTreeItems();
                          }
    break;
  case Portfolio::CurrencySummary: {
    ItemPortfolioCurrencySummary* p( new ItemPortfolioCurrencySummary( pPortfolio ) );
    m_setItems.insert( p );
    m_mapPortfolios.insert( mapPortfolios_t::value_type( idPortfolio, p ) );
    //m_mapUnattachedTreeItems.insert( mapUnattachedTreeItems_t::value_type( p->GetID(), p ) );
    m_setUnattachedTreeItems.insert( setUnattachedTreeItems_t::value_type( p ) );
    BuildTreeFromUnattachedTreeItems();
                                   }
    break;
  case Portfolio::Standard:
  case Portfolio::Basket:
  case Portfolio::MultiLeggedPosition: {
    ItemPortfolio* p( new ItemPortfolio( pPortfolio ) );
    m_setItems.insert( p );
    m_mapPortfolios.insert( mapPortfolios_t::value_type( idPortfolio, p ) );
    //m_mapUnattachedTreeItems.insert( mapUnattachedTreeItems_t::value_type( p->GetID(), p ) );
    m_setUnattachedTreeItems.insert( setUnattachedTreeItems_t::value_type( p ) );
    BuildTreeFromUnattachedTreeItems();
                                       }
    break;
  default:  throw std::runtime_error( "HandleOnPortfolioAdded doesn't know new Portfolio type" );
  }
}

void ModelPortfolioPositionOrderExecution::BuildTreeFromUnattachedTreeItems( void ) {
  bool bTreeUpdated;
  do { // process map multiple times 
    bTreeUpdated = false;
    // the for loop may require rework, iter1 is invalidated at loop end, so may cause issue in for loop
    // or restart map when an entry is erased
    //for ( setUnattachedTreeItems_t::iterator iter1 = m_setUnattachedTreeItems.begin(); m_setUnattachedTreeItems.end() != iter1; ++iter1 ) {
    for (auto dvib: m_setUnattachedTreeItems ) {
      //DataViewItemPortfolio& dv1( *iter1->second );
      idPortfolio_t idOwner = dvib->GetPtr()->GetRow().idOwner;
      assert( "" != idOwner );
      mapPortfolios_t::iterator iter2 = m_mapPortfolios.find( idOwner );
      if ( m_mapPortfolios.end() != iter2 ) { // skip if parent hasn't been installed yet
        //DataViewItemPortfolio& dv2( *iter2->second );
        dvib->SetParent( iter2->second );
        switch ( iter2->second->GetModelType() ) {
          case EMTPortfolioMaster:
            dynamic_cast<ItemPortfolioMaster*>( iter2->second )
              ->setItemPortfolioCurrencySummary.insert( setItemsPortfolio_t::value_type( dvib ) );
            break;
          case EMTPortfolioCurrency:
            dynamic_cast<ItemPortfolioCurrencySummary*>( iter2->second )
              ->setItemPortfolio.insert( setItemsPortfolio_t::value_type( dvib ) );
            break;
          case EMTPortfolio:
            dynamic_cast<ItemPortfolio*>( iter2->second )
              ->setItemPortfolio.insert( setItemsPortfolio_t::value_type( dvib ) );
            break;
          default: 
            throw std::runtime_error( "BuildTreeFromUnattachedTreeItems has no enumeration" );
        }
        //ItemChanged( dv2 );
        ItemChanged( wxDataViewItem( dvib ) );
        //ItemAdded( dv2, dv1 ); 
        //ItemAdded( wxDataViewItem( iter2->second ), wxDataViewItem( dvib ) ); // <-- 20170910 issues here: segfault -- need to redo code: wxDataViewItem( pPortfolioInfo )
        //ItemAdded( m_itemNull, *iter1->second );
        //m_mapUnattachedTreeItems.erase( iter1 );
        m_setUnattachedTreeItems.erase( dvib );
        bTreeUpdated = true;
        break;
      }
    }
  } while ( bTreeUpdated );
}

void ModelPortfolioPositionOrderExecution::HandleOnPortfolioUpdated( pPortfolio_t& ) {
}

void ModelPortfolioPositionOrderExecution::HandleOnPortfolioDeleted( const idPortfolio_t& ) {
}

void ModelPortfolioPositionOrderExecution::HandleOnPositionAdded( pPosition_t& ) {
}

void ModelPortfolioPositionOrderExecution::HandleOnPositionUpdated( pPosition_t& ) {
}

void ModelPortfolioPositionOrderExecution::HandleOnPositionDeleted( const idPosition_t& ) {
}


void ModelPortfolioPositionOrderExecution::HandleLoadMasterPortfolio( const idPortfolio_t& idPortfolio ) {
  m_pItemPortfolioMaster = new ItemPortfolioMaster( m_PortfolioManager.GetPortfolio( idPortfolio ) );
  m_setItems.insert( setItems_t::value_type( m_pItemPortfolioMaster ) );
  ItemAdded( wxDataViewItem( nullptr ), wxDataViewItem( m_pItemPortfolioMaster ) );
  //ItemChanged( *m_pItemPortfolioMaster );
}

bool ModelPortfolioPositionOrderExecution::IsContainer(	const wxDataViewItem&	item ) const {
  bool bReturn = false;
  if ( 0 == item.GetID() ) {
    bReturn = true;
  }
  else {
    setItems_t::const_iterator iter = m_setItems.find( reinterpret_cast<DataViewItemBase*>( item.GetID() ) );
    assert( m_setItems.end() != iter );
    DataViewItemBase* dvib = (*iter);
    bReturn = dvib->IsContainer();
  }
  return bReturn;
}

wxDataViewItem ModelPortfolioPositionOrderExecution::GetParent( const wxDataViewItem& item ) const {
  if ( !item.IsOk() )
    return wxDataViewItem(0);
  setItems_t::const_iterator iter = m_setItems.find( reinterpret_cast<DataViewItemBase*>( item.GetID() ) );
  assert( m_setItems.end() != iter );
  DataViewItemBase* pParent = (*iter)->GetParent();
  return wxDataViewItem( pParent );
}

unsigned int ModelPortfolioPositionOrderExecution::GetChildren(	const wxDataViewItem& item, wxDataViewItemArray& children	) const {
  // with columns this gets called without having to scan
  unsigned int count( 0 );
  if ( 0 == item.GetID() ) {
    if ( 0 != m_pItemPortfolioMaster ) {
      children.Add( wxDataViewItem( m_pItemPortfolioMaster ) );
      count = 1;  // fix this, if multiple children at root
    }
  }
  else {
    setItems_t::const_iterator iter = m_setItems.find( reinterpret_cast<DataViewItemBase*>( item.GetID() ) );
    assert( m_setItems.end() != iter );
    switch ( (*iter)->GetModelType() ) {
    case EMTPortfolioMaster: {
      setItemsPortfolio_t& set( dynamic_cast<ItemPortfolioMaster*>( *iter )->setItemPortfolioCurrencySummary );
      for ( setItemsPortfolio_t::const_iterator iter = set.begin(); set.end() != iter; ++iter ) {
        children.Add( wxDataViewItem( *iter ) );
        ++count;
      }
                           }
      break;
    case EMTPortfolioCurrency: {
      setItemsPortfolio_t& set1( dynamic_cast<ItemPortfolioCurrencySummary*>(*iter)->setItemPortfolio );
      for ( setItemsPortfolio_t::const_iterator iter = set1.begin(); set1.end() != iter; ++iter ) {
        children.Add( wxDataViewItem( *iter ) );
        ++count;
      }
      setItemsPosition_t& set2( dynamic_cast<ItemPortfolioCurrencySummary*>(*iter)->setItemPosition );
      for ( setItemsPosition_t::const_iterator iter = set2.begin(); set2.end() != iter; ++iter ) {
        children.Add( wxDataViewItem( *iter ) );
        ++count;
      }
                           }
      break;
    case EMTPortfolio: {
      setItemsPortfolio_t& set1( dynamic_cast<ItemPortfolioCurrencySummary*>(*iter)->setItemPortfolio );
      for ( setItemsPortfolio_t::const_iterator iter = set1.begin(); set1.end() != iter; ++iter ) {
        children.Add( wxDataViewItem( *iter ) );
        ++count;
      }
      setItemsPosition_t& set2( dynamic_cast<ItemPortfolioCurrencySummary*>(*iter)->setItemPosition );
      for ( setItemsPosition_t::const_iterator iter = set2.begin(); set2.end() != iter; ++iter ) {
        children.Add( wxDataViewItem( *iter ) );
        ++count;
      }
                           }
      break;
    case EMTPosition: {
      setItemsOrder_t& set1( dynamic_cast<ItemPosition*>(*iter)->setItemOrder );
      for ( setItemsOrder_t::const_iterator iter = set1.begin(); set1.end() != iter; ++iter ) {
        children.Add( wxDataViewItem( *iter ) );
        ++count;
      }
                    }
      break;
    case EMTOrder: {
      setItemsExecution_t& set1( dynamic_cast<ItemOrder*>(*iter)->setItemExecution );
      for ( setItemsExecution_t::const_iterator iter = set1.begin(); set1.end() != iter; ++iter ) {
        children.Add( wxDataViewItem( *iter ) );
        ++count;
      }
                    }
      break;
    case EMTExecution:
      throw std::runtime_error( "eExecution found, shouldn't be" );
      break;
    }
  }
  return count;
}

void ModelPortfolioPositionOrderExecution::GetValue( wxVariant& variant, const wxDataViewItem& item, unsigned int col	) const {
  setItems_t::const_iterator iter = m_setItems.find( reinterpret_cast<DataViewItemBase*>( item.GetID() ) );
  assert( iter != m_setItems.end() );
  (*iter)->AssignFirstColumn( variant );
}

void ModelPortfolioPositionOrderExecution::ClickedOnTreeItem( DataViewItemBase* pItem ) {
  setItems_t::const_iterator iter = m_setItems.find( pItem );
  assert( iter != m_setItems.end() );
  switch ( (*iter)->GetModelType() ) {
    case EMTPortfolioMaster:
      m_pModelExecution->ClearItems();
      m_pModelOrder->ClearItems();
      m_pModelPosition->ClearItems();
      m_pModelPortfolio->ClearItems();
      m_pModelPortfolio->AddPortfolioToModel( dynamic_cast<DataViewItemPortfolio*>( *iter ) );
      // add the currency summary portfolios as well?
      break;
    case EMTPortfolioCurrency:
      m_pModelExecution->ClearItems();
      m_pModelOrder->ClearItems();
      m_pModelPosition->ClearItems();
      m_pModelPortfolio->AddPortfolioToModel( dynamic_cast<DataViewItemPortfolio*>( *iter ) );
      // show all sub portfolios and positions?  or just in tree?
      break;
    case EMTPortfolio:
      m_pModelExecution->ClearItems();
      m_pModelOrder->ClearItems();
      m_pModelPosition->ClearItems();
      m_pModelPortfolio->ClearItems();
      m_pModelPortfolio->AddPortfolioToModel( dynamic_cast<DataViewItemPortfolio*>( *iter ) );
      // show sub portfolios and positions?  how to show sub portfolios as members?
      break;
    case EMTPosition:
      m_pModelExecution->ClearItems();
      m_pModelOrder->ClearItems();
      m_pModelPosition->ClearItems();
      // show orders?
      break;
    case EMTOrder:
      m_pModelExecution->ClearItems();
      m_pModelOrder->ClearItems();
      // show executions?
      break;
    case EMTExecution:
      m_pModelExecution->ClearItems();
      // not sure if we get here or not
      break;
    case EMTUnknown:
      assert( 0 );
      break;
  }
}

EModelType ModelPortfolioPositionOrderExecution::GetModelType( const wxDataViewItem& item ) {
  // replace other scattered code with a call to this method
  setItems_t::const_iterator iter = m_setItems.find( reinterpret_cast<DataViewItemBase*>( item.GetID() ) );
  assert( iter != m_setItems.end() );
  return ( (*iter)->GetModelType() );
}

} // namespace tf
} // namespace ou
