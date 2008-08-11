#include "StdAfx.h"

#include "BasketTradeModel.h"

#include "DbValueStream.h"
#include "CommonDatabaseFunctions.h"
#include "HDF5IterateGroups.h"
#include "HDF5DataManager.h"

#include <ostream>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//
// CBasketTradeModel
//

CBasketTradeModel::CBasketTradeModel( CProviderInterface *pDataProvider, CProviderInterface *pExecutionProvider )
//: m_pDataProvider( pDataProvider ), m_pExecutionProvider( pExecutionProvider )
{
  m_ModelInfo.pDataProvider = pDataProvider;
  m_ModelInfo.pExecutionProvider = pExecutionProvider;
  m_ModelInfo.pTreeView = m_ChartingContainer.GetTreeView();
}

CBasketTradeModel::~CBasketTradeModel(void) {
  mapBasketSymbols_t::iterator iter;
  for ( iter = m_mapBasketSymbols.begin(); iter != m_mapBasketSymbols.end(); ++iter ) {
    // todo: need to remove any events attached to this object as well
    iter->second->StopTrading();
    delete iter->second;
  }
  m_mapBasketSymbols.clear();
}

void CBasketTradeModel::AddSymbol(const std::string &sSymbolName, const std::string &sPath, const std::string &sStrategy) {
  mapBasketSymbols_t::iterator iter;
  iter = m_mapBasketSymbols.find( sSymbolName );
  if ( m_mapBasketSymbols.end() == iter ) {
    CBasketTradeSymbolInfo *pInfo 
      = new CBasketTradeSymbolInfo( sSymbolName, sPath, sStrategy );
    m_mapBasketSymbols.insert( pairBasketSymbolsEntry_t( sSymbolName, pInfo ) );
    OnBasketTradeSymbolInfoAddedToBasket( pInfo );
    std::cout << "Basket add for " << sSymbolName << " successful." << std::endl;
  }
  else {
    std::cout << "Basket add for " << sSymbolName << " failed, symbol already exists." << std::endl;
  }
}

void CBasketTradeModel::Prepare( ptime dtTradeDate, double dblFunds, bool bRTHOnly ) {
  int nTradeableSymbols = 0;
  int nSymbolCount = 0;
  double dblFundsPerSymbol = 0;
  double dblCostForEntry = 0;
  mapBasketSymbols_t::iterator iter; 
  m_ModelInfo.bRTH = bRTHOnly;
  m_ModelInfo.dtTradeDate = dtTradeDate;
  m_ModelInfo.dtRTHBgn = ptime( dtTradeDate.date(), time_duration( 10, 30, 00 ) );
  m_ModelInfo.dtOpenRangeBgn = m_ModelInfo.dtRTHBgn;
  m_ModelInfo.dtOpenRangeEnd = ptime( dtTradeDate.date(), time_duration( 10, 34, 0 ) );
  m_ModelInfo.dtEndActiveTrading = ptime( dtTradeDate.date(), time_duration( 16, 40, 0 ) );
  m_ModelInfo.dtBgnNoMoreTrades = ptime( dtTradeDate.date(), time_duration( 16, 42, 0 ) );
  m_ModelInfo.dtBgnCancelTrades = ptime( dtTradeDate.date(), time_duration( 16, 44, 0 ) );
  m_ModelInfo.dtBgnCloseTrades = ptime( dtTradeDate.date(), time_duration( 16, 46, 0 ) );
  m_ModelInfo.dtRTHEnd = ptime( dtTradeDate.date(), time_duration( 17, 00, 00 ) );
  try {
    for ( int nLoopCount = 1; nLoopCount <= 2; ++nLoopCount ) {
      double dblTotalCostForEntry = 0;
      switch ( nLoopCount ) {
        case 1:
          nSymbolCount = m_mapBasketSymbols.size();
          dblFundsPerSymbol = dblFunds / nSymbolCount;
          m_ModelInfo.nCalcStep = CBasketTradeSymbolInfo::structCommonModelInformation::Prelim;
          break;
        case 2: 
          nSymbolCount = nTradeableSymbols;
          dblFundsPerSymbol = dblFunds / nSymbolCount;
          m_ModelInfo.nCalcStep = CBasketTradeSymbolInfo::structCommonModelInformation::Final;
          break;
      }
      for( iter = m_mapBasketSymbols.begin(); iter != m_mapBasketSymbols.end(); ++iter ) {
        switch ( nLoopCount ) {
          case 1:
            m_ModelInfo.dblFunds = dblFundsPerSymbol;
            iter->second->CalculateTrade( &m_ModelInfo );
            dblCostForEntry = iter->second->GetProposedEntryCost();
            if ( 0 != dblCostForEntry ) {
              ++nTradeableSymbols;
              dblTotalCostForEntry += dblCostForEntry;
            }
            break;
          case 2:
            dblCostForEntry = iter->second->GetProposedEntryCost();  // was set on last loop through
            if ( 0 != dblCostForEntry ) {
              m_ModelInfo.dblFunds = dblFundsPerSymbol;
              iter->second->CalculateTrade( &m_ModelInfo );
              dblCostForEntry = iter->second->GetProposedEntryCost();
              dblTotalCostForEntry += dblCostForEntry;
              iter->second->StartTrading();
            }
            break;
        }
      }
      std::cout << "# Symbols: " << nSymbolCount << ", Total Cost of Entry: " << dblTotalCostForEntry << std::endl;
    }
  }
  catch (...) {
    std::cout << "error somewhere" << std::endl;
  }
}

void CBasketTradeModel::WriteBasketList() {

  CDbValueStream strm;
  std::ostream out(&strm);
  unsigned long key = 0;

  strm.Truncate();
  for( mapBasketSymbols_t::iterator iter = m_mapBasketSymbols.begin(); iter != m_mapBasketSymbols.end(); ++iter ) {
    out.flush();
    iter->second->StreamSymbolInfo( &out );
    strm.Save( &key, sizeof( key ) );
    ++key;
  }
}

void CBasketTradeModel::ReadBasketList() {
  //std::stringstream in;
  //CBasketTradeSymbolInfo *pBasket = new CBasketTradeSymbolInfo( &in, m_pExecutionProvider );
}

void CBasketTradeModel::ReadBasketData( const std::string &sGroupName ) {  // directory of symbol trades to read
  if ( 0 != m_mapBasketSymbols.size() ) {
    std::cout << "Nothing added, symbols already in basket model" << std::endl;
  }
  else {
    try {
      std::cout << "** Symbol list may have been limited in 'Process' ***" << std::endl;
      HDF5IterateGroups<CBasketTradeModel> control;
      int result = control.Start( sGroupName, this );
    }
    catch ( H5::FileIException e ) {
      std::cout << "problem with group iteration with file" << e.getDetailMsg() << std::endl;
      e.walkErrorStack( H5E_WALK_DOWNWARD, (H5E_walk2_t) &CHDF5DataManager::PrintH5ErrorStackItem, 0 );
    }
    catch ( H5::GroupIException e ) {
      std::cout << "problem with group interation with group" << e.getDetailMsg() << std::endl;
      e.walkErrorStack( H5E_WALK_DOWNWARD, (H5E_walk2_t) &CHDF5DataManager::PrintH5ErrorStackItem, 0 );
    }
    catch (...) {
      std::cout << "problems with group iteration" << std::endl;
    }
  }
}

void CBasketTradeModel::WriteBasketData( const std::string &sGroupName ) {
  for( mapBasketSymbols_t::iterator iter = m_mapBasketSymbols.begin(); iter != m_mapBasketSymbols.end(); ++iter ) {
    iter->second->WriteTradesAndQuotes( sGroupName );
  }
}

void CBasketTradeModel::Process(const std::string &sObjectName, const std::string &sObjectPath) {
  static std::string rSymbols[] = { "SY", "MER", "LVS", "JRCC", "" };
//  int ix = 0;
//  while ( rSymbols[ix] != "" ) {
//    if ( rSymbols[ix] == sObjectName ) {
      std::string sPath;
      CHDF5DataManager::DailyBarPath( sObjectName, sPath );
      AddSymbol( sObjectName, sPath, "archive" );
//      break;
//    }
//    ++ix;
//  }
}
