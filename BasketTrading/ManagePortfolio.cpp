/************************************************************************
 * Copyright(c) 2012, One Unified. All rights reserved.                 *
 * email: info@oneunified.net                                           *
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

#include <boost/foreach.hpp>

#include <OUCommon/TimeSource.h>

#include "ManagePortfolio.h"

ManagePortfolio::ManagePortfolio( void )
  : m_dblPortfolioCashToTrade( 100000.0 ), m_dblPortfolioMargin( 0.25 ), m_nSharesTrading( 0 )
{

  std::stringstream ss;
  ss.str( "" );
  ss << ou::TimeSource::Instance().External();
  //m_sTSDataStreamStarted = "/app/BasketTrading/" + ss.str();  
  // will need to make this generic if need some for multiple providers.
  m_sTSDataStreamStarted = ss.str();  // will need to make this generic if need some for multiple providers.

}

ManagePortfolio::~ManagePortfolio(void) {
  BOOST_FOREACH( mapPositions_pair_t pair, m_mapPositions ) {
    delete pair.second;
  }
}

void ManagePortfolio::Start( pPortfolio_t pPortfolio, pProvider_t pExec, pProvider_t pData1, pProvider_t pData2 ) {

  assert( 0 != m_mapPositions.size() );

  m_pPortfolio = pPortfolio;

  m_pExec = pExec;
  m_pData1 = pData1;
  m_pData2 = pData2;


  // first pass: to get rough idea of which can be traded given our funding level
  double dblAmountToTradePerInstrument = ( m_dblPortfolioCashToTrade / m_dblPortfolioMargin ) / m_mapPositions.size();

  unsigned int cntToBeTraded = 0;
  BOOST_FOREACH( mapPositions_pair_t pair, m_mapPositions ) {
    if ( 200 <= pair.second->CalcShareCount( dblAmountToTradePerInstrument ) ) {
      cntToBeTraded++;
      pair.second->ToBeTraded() = true;
    }
    else {
      pair.second->ToBeTraded() = false;
    }
  }

  // second pass: start trading with the ones that we can
  dblAmountToTradePerInstrument = ( m_dblPortfolioCashToTrade / m_dblPortfolioMargin ) / cntToBeTraded;
  switch ( pExec->ID() ) {
    case ou::tf::keytypes::EProviderIB:
      m_pIB = boost::dynamic_pointer_cast<ou::tf::IBTWS>( pExec );
      BOOST_FOREACH( mapPositions_pair_t pair, m_mapPositions ) {
        if ( pair.second->ToBeTraded() ) {

          pair.second->SetFundsToTrade( dblAmountToTradePerInstrument );
          m_nSharesTrading += pair.second->CalcShareCount( dblAmountToTradePerInstrument );

          ou::tf::IBTWS::Contract contract;
          contract.currency = "USD";
          contract.exchange = "SMART";  
          contract.secType = "STK";
          contract.symbol = pair.first;
          // IB responds only when symbol is found, bad symbols will not illicit a response
          m_pIB->RequestContractDetails( contract, MakeDelegate( this, &ManagePortfolio::HandleIBContractDetails ), MakeDelegate( this, &ManagePortfolio::HandleIBContractDetailsDone ) );
        }
      }
      break;
    default:
      std::cout << "cant' get symbols" << std::endl;
    }
  std::cout << "#Shares to be traded: " << m_nSharesTrading << std::endl;
}

void ManagePortfolio::Stop( void ) {
  BOOST_FOREACH( mapPositions_pair_t pair, m_mapPositions ) {
    if ( pair.second->ToBeTraded() ) {
      pair.second->Stop();
    }
  }
}

void ManagePortfolio::AddSymbol( const std::string& sName, const ou::tf::Bar& bar ) {
  assert( m_mapPositions.end() == m_mapPositions.find( sName ) );
  m_mapPositions[ sName ] = new ManagePosition( sName, bar );
}

void ManagePortfolio::SaveSeries( const std::string& sPrefix ) {
  std::cout << "Saving ... ";
  std::string sPath( sPrefix + m_sTSDataStreamStarted );
  BOOST_FOREACH( mapPositions_pair_t pair, m_mapPositions ) {
    pair.second->SaveSeries( sPath );
  }
  std::cout << "done." << std::endl;
}

// comes in on a different thread, so no gui operations
void ManagePortfolio::HandleIBContractDetails( const ou::tf::IBTWS::ContractDetails& details, pInstrument_t& pInstrument ) {
  mapPositions_iter_t iter = m_mapPositions.find( pInstrument->GetInstrumentName() );
  assert( m_mapPositions.end() != iter );
  //pPosition_t pPosition( new ou::tf::CPosition( pInstrument, m_pIB, m_pData1 ) );
  pPosition_t pPosition = ou::tf::PortfolioManager::Instance().ConstructPosition( 
    m_pPortfolio->Id(), pInstrument->GetInstrumentName(), "Basket", 
    "ib01", "iq01", m_pExec, m_pData1, pInstrument );
  iter->second->SetPosition( pPosition );
  iter->second->Start();
//  m_md.data.tdMarketOpen = m_pInstrument->GetTimeTrading().begin().time_of_day();
//  m_md.data.tdMarketClosed = m_pInstrument->GetTimeTrading().end().time_of_day();
}

void ManagePortfolio::HandleIBContractDetailsDone( void ) {
//  StartWatch();
}


