#pragma once

#include "..\LibTrading\ProviderInterface.h"
#include "BasketTradeModel.h"
#include "BasketTradeViewPosition.h"
#include "BasketTradeViewSymbolList.h"
#include "BasketTradeController.h"
#include "BasketTradeViewDialog.h"

#include <string>

class CBasketTradeContainer {
public:
  CBasketTradeContainer( CProviderInterface *pDataProvider, CProviderInterface *pExecutionProvider );
  ~CBasketTradeContainer(void);
  void AddSymbol( const std::string &sSymbolName, const std::string &sPath, const std::string &sStrategy );
  void Prepare( ptime dtTradeDate, double dblFunds, bool bRTHOnly ) { m_pModel->Prepare( dtTradeDate, dblFunds, bRTHOnly ); };
  void SaveBasket( void ) { m_pModel->WriteBasketToDatabase(); };
  void LoadBasket( void ) { m_pModel->ReadBasketFromDatabase(); };

protected:
  CProviderInterface *m_pDataProvider;
  CProviderInterface *m_pExecutionProvider;

  CBasketTradeModel *m_pModel;
  CBasketTradeViewPosition *m_pVuPosition;
  CBasketTradeViewSymbolList *m_pVuSymbols;
  CBasketTradeViewDialog *m_pVuDialog;
  CBasketTradeController *m_pController;
private:
};
