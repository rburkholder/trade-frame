#include "StdAfx.h"
#include "BasketTradeContainer.h"

// need DataProvider, ExecutionProvider
// Trades data
// Need market orders

// Need DataProvider: trades data
// Need ExecutionProvider: market order, execution report, execution summary

CBasketTradeContainer::CBasketTradeContainer( CProviderInterface *pDataProvider, CProviderInterface *pExecutionProvider ) 
: m_pDataProvider( pDataProvider ), m_pExecutionProvider( pExecutionProvider ),
  m_pVuPosition( NULL ), m_pVuSymbols( NULL ), m_pVuDialog( NULL ), m_pController( NULL )
{
  m_pModel = new CBasketTradeModel( pDataProvider, pExecutionProvider );
  m_pVuDialog = new CBasketTradeViewDialog( );
  m_pController = new CBasketTradeController( );
  std::cout << "Basket Waiting" << std::endl;
}

CBasketTradeContainer::~CBasketTradeContainer(void) {
  if ( NULL != m_pController ) {
    delete m_pController;
    m_pController = NULL;
  }
  if ( NULL != m_pVuDialog ) {
    delete m_pVuDialog;
    m_pVuDialog = NULL;
  }
  if ( NULL != m_pModel ) {
    delete m_pModel;
    m_pModel = NULL;
  }
}

void CBasketTradeContainer::AddSymbol( const std::string &sSymbolName, const std::string &sPath, const std::string &sStrategy) {
  assert( NULL != m_pModel );
  m_pModel->AddSymbol( sSymbolName, sPath, sStrategy );
}
