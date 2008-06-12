#pragma once

#include "..\LibTrading\ProviderInterface.h"
#include "BasketTradeSymbolInfo.h"

#include <map>
#include <string>


class CBasketTradeModel {
public:
  CBasketTradeModel( CProviderInterface *pDataProvider, CProviderInterface *pExecutionProvider );
  ~CBasketTradeModel(void);
  void AddSymbol( const std::string &sSymbolName, const std::string &sPath, const std::string &sStrategy  );
  void Prepare( ptime dtTradeDate, double dblFunds, bool bRTHOnly );
  void WriteBasketToDatabase( void );
  void ReadBasketFromDatabase( void );
protected:
  typedef std::map<std::string, CBasketTradeSymbolInfo*> mapBasketSymbols_t;
  mapBasketSymbols_t m_mapBasketSymbols;
  typedef pair<std::string, CBasketTradeSymbolInfo*> pairBasketSymbolsEntry_t;
  CProviderInterface *m_pDataProvider;
  CProviderInterface *m_pExecutionProvider;
private:
};
