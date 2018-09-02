/************************************************************************
 * Copyright(c) 2010, One Unified. All rights reserved.                 *
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

/* 
 * File:   ManageStrategy.cpp
 * Author: raymond@burkholder.net
 * 
 * Created on August 26, 2018, 6:46 PM
 */

#include "ManageStrategy.h"

ManageStrategy::ManageStrategy( const std::string& sUnderlying, const ou::tf::Bar& barPriorDaily, fConstructPosition_t fConstructPosition, fLoadSymbolDefinitions_t fLoadSymbolDefinitions )
: m_sUnderlying( sUnderlying ), m_barPriorDaily( barPriorDaily ), m_fConstructPosition( fConstructPosition ), m_fLoadSymbolDefinitions( fLoadSymbolDefinitions ),
  m_nUnderlyingSharesToTrade {}
{ 
  assert( nullptr != m_fConstructPosition );
  assert( nullptr != m_fLoadSymbolDefinitions );
  
  m_pPositionUnderlying = m_fConstructPosition( sUnderlying );
  
  m_fLoadSymbolDefinitions( sUnderlying, [this](const ou::tf::iqfeed::MarketSymbol::TableRowDef& row){  // these are iqfeed based symbol names
  });
  
  // choose an appropriate put at least three days out and create a position
  // but can't do this until after opening bell until we have a current quote range
  //m_PositionPut_Current = m_fConstructPosition( sPut );
  
}

ManageStrategy::~ManageStrategy( ) { 
}

ou::tf::DatedDatum::volume_t ManageStrategy::CalcShareCount( double dblFunds ) {
  m_nOptionContractsToTrade = ( (volume_t)( dblFunds / m_barPriorDaily.Close() ) )/ 100;
  m_nUnderlyingSharesToTrade = m_nOptionContractsToTrade * 100;  // round down to nearest 100
  //return ( static_cast<ou::tf::DatedDatum::volume_t>( dblFunds / m_barInfo.Close() ) / 100 ) * 100;  // round down to nearest 100
  return m_nUnderlyingSharesToTrade;
}

void ManageStrategy::HandleBellHeard( void ) {
  
}

void ManageStrategy::HandleQuote( const ou::tf::Quote& quote ) {
  
}

void ManageStrategy::HandleTrade( const ou::tf::Trade& trade ) {
  
}

void ManageStrategy::HandleRHTrading( const ou::tf::Quote& quote ) {
  
}

void ManageStrategy::HandleCancel( void ) {
  
}

void ManageStrategy::HandleGoNeutral( void ) {
  
}

void ManageStrategy::HandleAfterRH( const ou::tf::Quote& quote ) {
  
}

