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

#include <algorithm>

#include <OUCommon/TimeSource.h>

#include <TFIQFeed/BuildInstrument.h>

#include <TFTrading/InstrumentManager.h>

#include "MasterPortfolio.h"

MasterPortfolio::MasterPortfolio( 
    fGatherOptionDefinitions_t fGatherOptionDefinitions,
    fGetTableRowDef_t fGetTableRowDef
    )
  : m_dblPortfolioCashToTrade( 100000.0 ), m_dblPortfolioMargin( 0.25 ), m_nSharesTrading( 0 ),
    m_fOptionNamesByUnderlying( std::move( fGatherOptionDefinitions ) ),
    m_fGetTableRowDef( std::move( fGetTableRowDef ) )
{
  assert( nullptr != m_fOptionNamesByUnderlying );
  assert( nullptr != m_fGetTableRowDef );
  
  std::stringstream ss;
  ss.str( "" );
  ss << ou::TimeSource::Instance().External();
  // will need to make this generic if need some for multiple providers.
  m_sTSDataStreamStarted = ss.str();  // will need to make this generic if need some for multiple providers.
  
  m_pOptionEngine.reset( new ou::tf::option::Engine( m_libor ) );
  m_pOptionEngine->m_fBuildWatch 
    = [this](pInstrument_t pInstrument)->pWatch_t {
        ou::tf::Watch::pWatch_t pWatch( new ou::tf::Watch( pInstrument, m_pData1 ) );
        return pWatch;
      };
  m_pOptionEngine->m_fBuildOption 
    = [this](pInstrument_t pInstrument)->pOption_t {
        ou::tf::option::Option::pOption_t pOption( new ou::tf::option::Option( pInstrument, m_pData1 ) );
        return pOption;
      };
}

MasterPortfolio::~MasterPortfolio(void) {
  m_mapStrategy.clear();
}

void MasterPortfolio::Start( pPortfolio_t pMasterPortfolio, pProvider_t pExec, pProvider_t pData1, pProvider_t pData2 ) {

  assert( 0 != m_mapStrategy.size() );

  m_pMasterPortfolio = pMasterPortfolio;

  m_pExec = pExec;
  m_pData1 = pData1;
  m_pData2 = pData2;
  
  switch ( pExec->ID() ) {
    case ou::tf::keytypes::EProviderIB:
      m_pIB = boost::dynamic_pointer_cast<ou::tf::IBTWS>( pExec );
      break;
  }
  
  switch ( pData1->ID() ) {
    case ou::tf::keytypes::EProviderIQF:
      m_pIQ = boost::dynamic_pointer_cast<ou::tf::IQFeedProvider>( pData1 );
      break;
    default:
      assert( 0 ); // need the iqfeed provider
  }
  
  m_libor.SetWatchOn( m_pIQ );

  // first pass: to get rough idea of which can be traded given our funding level
  double dblAmountToTradePerInstrument = ( m_dblPortfolioCashToTrade / m_dblPortfolioMargin ) / m_mapStrategy.size();

  unsigned int cntToBeTraded = 0;
  
  std::for_each( m_mapStrategy.begin(), m_mapStrategy.end(), 
                [&cntToBeTraded, dblAmountToTradePerInstrument](mapStrategy_t::value_type& pair){
                  if ( 200 <= pair.second->CalcShareCount( dblAmountToTradePerInstrument ) ) {
                    cntToBeTraded++;
                    pair.second->ToBeTraded() = true;
                  }
                  else {
                    pair.second->ToBeTraded() = false;
                  }
                } );
  
  // second pass: start trading with the ones that we can
  dblAmountToTradePerInstrument = ( m_dblPortfolioCashToTrade / m_dblPortfolioMargin ) / cntToBeTraded;
  
  if ( nullptr != m_pIB.get() ) {
      std::for_each( m_mapStrategy.begin(), m_mapStrategy.end(), 
                    [dblAmountToTradePerInstrument, this](mapStrategy_t::value_type& pair){
                      if ( pair.second->ToBeTraded() ) {

                        pair.second->SetFundsToTrade( dblAmountToTradePerInstrument );
                        m_nSharesTrading += pair.second->CalcShareCount( dblAmountToTradePerInstrument );

                        ou::tf::IBTWS::Contract contract;
                        contract.currency = "USD";
                        contract.exchange = "SMART";  
                        contract.secType = "STK";
                        contract.symbol = pair.first;
                        // IB responds only when symbol is found, bad symbols will not illicit a response
                        m_pIB->RequestContractDetails( 
                          contract, 
                          MakeDelegate( this, &MasterPortfolio::HandleIBContractDetails ), MakeDelegate( this, &MasterPortfolio::HandleIBContractDetailsDone ) 
                          );
                      }
                    } );
  }
  else {
    std::cout << "can't get symbols" << std::endl;
  }
  
  std::cout << "#Shares to be traded: " << m_nSharesTrading << std::endl;
}

void MasterPortfolio::Stop( void ) {
  std::for_each( m_mapStrategy.begin(), m_mapStrategy.end(), 
                [](mapStrategy_t::value_type& pair){
                  if ( pair.second->ToBeTraded() ) {
                    pair.second->Stop();
                  }
                } );  
  m_libor.SetWatchOff();                
}

void MasterPortfolio::AddSymbol( const std::string& sName, const ou::tf::Bar& bar, double dblStop ) {
  assert( m_mapStrategy.end() == m_mapStrategy.find( sName ) );
  pManageStrategy_t pManageStrategy;
  pManageStrategy.reset( new ManageStrategy( 
        sName, bar,
    // ManageStrategy::fGatherOptionDefinitions_t
        m_fOptionNamesByUnderlying,
    // ManageStrategy::fConstructPositionUnderlying_t
        [this](const std::string& sIQFeedEquityName)->ManageStrategy::pPosition_t{
              const trd_t& trd( m_fGetTableRowDef( sIQFeedEquityName ) ); // TODO: check for errors
              
              pInstrument_t pEquityInstrument;
              ou::tf::InstrumentManager& im( ou::tf::InstrumentManager::GlobalInstance().Instance() );
              
              if ( im.Exists( sIQFeedEquityName, pEquityInstrument ) ) {
                // pEquityInstrument has been populated
              }
              else {
                pEquityInstrument = ou::tf::iqfeed::BuildInstrument( sIQFeedEquityName, trd );  // builds equity option, may not build futures option
                if ( nullptr != m_pIB.get() ) {
                    m_pIB->RequestContractDetails( 
                      sIQFeedEquityName, pEquityInstrument,
                      [this](const ou::tf::IBTWS::ContractDetails& details, pInstrument_t& pInstrument){
                        // the contract details fill in the contract in the instrument, which can then be passed back to the caller 
                        //   as a fully defined, registered instrument
                        assert( 0 != pInstrument->GetContract() );
                        ou::tf::InstrumentManager& im( ou::tf::InstrumentManager::GlobalInstance().Instance() ); 
                        im.Register( pInstrument );  // is a CallAfter required, or can this run in a thread?
                      }, 
                      nullptr  // request complete doesn't need a function
                      );
                }
              }
              
              pPosition_t pPosition( new ou::tf::Position ( m_pOptionEngine->m_fBuildWatch( pEquityInstrument ), m_pExec ) );
              return pPosition;
          },
    // ManageStrategy::fConstructPositionOption_t
        [this]( const pInstrument_t pUnderlyingInstrument, const std::string& sIQFeedOptionName )->ManageStrategy::pPosition_t{
              const trd_t& trd( m_fGetTableRowDef( sIQFeedOptionName ) ); // TODO: check for errors
              
              std::string sGenericOptionName 
                = ou::tf::Instrument::BuildGenericOptionName( pUnderlyingInstrument->GetInstrumentName(), trd.eOptionSide, trd.nYear, trd.nMonth, trd.nDay, trd.dblStrike );

              pInstrument_t pOptionInstrument;
              ou::tf::InstrumentManager& im( ou::tf::InstrumentManager::GlobalInstance().Instance() );
              
              if ( im.Exists( sGenericOptionName, pOptionInstrument ) ) {
                // pOptionInstrument has been populated
              }
              else {
                pOptionInstrument = ou::tf::iqfeed::BuildInstrument( sGenericOptionName, trd );  // builds equity option, may not build futures option
                if ( nullptr != m_pIB.get() ) {
                    m_pIB->RequestContractDetails( 
                      pUnderlyingInstrument->GetInstrumentName(), pOptionInstrument,
                      [this](const ou::tf::IBTWS::ContractDetails& details, pInstrument_t& pInstrument){
                        // the contract details fill in the contract in the instrument, which can then be passed back to the caller 
                        //   as a fully defined, registered instrument
                        assert( 0 != pInstrument->GetContract() );
                        ou::tf::InstrumentManager& im( ou::tf::InstrumentManager::GlobalInstance().Instance() ); 
                        im.Register( pInstrument );  // is a CallAfter required, or can this run in a thread?
                      }, 
                      nullptr  // request complete doesn't need a function
                      );
                }
              }
              
              pPosition_t pPosition( new ou::tf::Position ( m_pOptionEngine->m_fBuildOption( pOptionInstrument ), m_pExec ) );
              return pPosition;
            }
        )
      );
    m_mapStrategy[ sName ] = std::move( pManageStrategy );        
}

void MasterPortfolio::SaveSeries( const std::string& sPrefix ) {
  std::cout << "Saving ... ";
  std::string sPath( sPrefix + m_sTSDataStreamStarted );
  m_libor.SaveSeries( sPath );                
  std::for_each(m_mapStrategy.begin(), m_mapStrategy.end(), 
                [&sPath](mapStrategy_t::value_type& pair){
                  pair.second->SaveSeries( sPath );
                } );
  std::cout << "done." << std::endl;
}

// comes in on a different thread, so no gui operations
void MasterPortfolio::HandleIBContractDetails( const ou::tf::IBTWS::ContractDetails& details, pInstrument_t& pInstrument ) {
  mapStrategy_t::iterator iter = m_mapStrategy.find( pInstrument->GetInstrumentName() );
  assert( m_mapStrategy.end() != iter );
  //pPosition_t pPosition( new ou::tf::CPosition( pInstrument, m_pIB, m_pData1 ) );
  pPosition_t pPosition = ou::tf::PortfolioManager::Instance().ConstructPosition( 
    m_pMasterPortfolio->Id(), pInstrument->GetInstrumentName(), "Basket", 
    "ib01", "iq01", m_pExec, m_pData1, pInstrument );
  //iter->second->SetPosition( pPosition );
  iter->second->Start();
//  m_md.data.tdMarketOpen = m_pInstrument->GetTimeTrading().begin().time_of_day();
//  m_md.data.tdMarketClosed = m_pInstrument->GetTimeTrading().end().time_of_day();
}

void MasterPortfolio::HandleIBContractDetailsDone( void ) {
//  StartWatch();
}


