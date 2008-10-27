#pragma once

#include "ProviderInterface.h"
#include "BasketTradeSymbolBase.h"
#include "Delegate.h"
#include "ChartingContainer.h"

#include <map>
#include <string>

class CBasketTradeModel {
public:
  CBasketTradeModel( CProviderInterface *pDataProvider, CProviderInterface *pExecutionProvider );
  ~CBasketTradeModel(void);
  void AddSymbol( const std::string &sSymbolName, const std::string &sPath, const std::string &sStrategy  );
  void Prepare( ptime dtTradeDate, double dblFunds, bool bRTHOnly );
  void WriteBasketList( void );
  void ReadBasketList( void );
  void WriteBasketData( const std::string &sGroupName );
  void ReadBasketData( const std::string &sGroupName );
  Delegate<CBasketTradeSymbolBase*> OnBasketTradeSymbolInfoAddedToBasket;
  void Process( const std::string &sObjectName, const std::string &sObjectPath );
protected:
  typedef std::map<std::string, CBasketTradeSymbolBase*> mapBasketSymbols_t;
  mapBasketSymbols_t m_mapBasketSymbols;
  typedef pair<std::string, CBasketTradeSymbolBase*> pairBasketSymbolsEntry_t;
  CBasketTradeSymbolBase::structCommonModelInformation m_ModelInfo;
  CChartingContainer m_ChartingContainer;
  //CProviderInterface *m_pDataProvider;
  //CProviderInterface *m_pExecutionProvider;
private:
};
