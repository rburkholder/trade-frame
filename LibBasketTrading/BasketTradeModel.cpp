#include "StdAfx.h"
#include "BasketTradeModel.h"


//
// CBasketTradeModel
//

CBasketTradeModel::CBasketTradeModel( CProviderInterface *pDataProvider, CProviderInterface *pExecutionProvider )
: m_pDataProvider( pDataProvider ),
    m_pExecutionProvider( pExecutionProvider )
{
}

CBasketTradeModel::~CBasketTradeModel(void) {
  m_mapBasketSymbols.clear();
}

void CBasketTradeModel::AddSymbol(const std::string &sSymbolName, const std::string &sPath, const std::string &sStrategy) {
  mapBasketSymbols_t::iterator iter;
  iter = m_mapBasketSymbols.find( sSymbolName );
  if ( m_mapBasketSymbols.end() == iter ) {
    m_mapBasketSymbols.insert( pairBasketSymbolsEntry_t( 
      sSymbolName, CBasketTradeSymbolInfo( sSymbolName, sPath, sStrategy ) ) );
    std::cout << "Basket add for " << sSymbolName << " successful." << std::endl;
  }
  else {
    std::cout << "Basket add for " << sSymbolName << " failed, symbol already exists." << std::endl;
  }
}

void CBasketTradeModel::Prepare(ptime dtTradeDate, double dblFunds) {
  mapBasketSymbols_t::iterator iter;
  double dblFundsPerSymbol = dblFunds / m_mapBasketSymbols.size();
  double dblCostForEntry = 0;
  for( iter = m_mapBasketSymbols.begin(); iter != m_mapBasketSymbols.end(); ++iter ) {
    iter->second.CalculateTrade( dtTradeDate, dblFundsPerSymbol );
    dblCostForEntry += iter->second.GetProposedEntryCost();
    m_pDataProvider->AddTradeHandler( iter->second.GetSymbolName(), MakeDelegate( &iter->second, &CBasketTradeSymbolInfo::HandleTrade ) );
  }
  std::cout << "Total Cost of Entry: " << dblCostForEntry << std::endl;
}
