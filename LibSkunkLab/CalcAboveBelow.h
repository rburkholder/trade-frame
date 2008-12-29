#pragma once

// From HDF5 file, calculates statistics for price range above open and below open

#include "ProviderInterface.h"
#include "Portfolio.h"
#include "Symbol.h"

#include <string>

class CCalcAboveBelow {
public:
  CCalcAboveBelow( const std::string &sSymbol, CProviderInterface *pDataProvider, CProviderInterface *pExecutionProvider );
  ~CCalcAboveBelow(void);
  void Start( void );   // pass in symbol name sometime
protected:
  void HandleUpdatePortfolioRecord( CPortfolio::UpdatePortfolioRecord_t );
  void HandleTrade( const CTrade &trade );
private:
  CProviderInterface *m_pDataProvider;
  CProviderInterface *m_pExecutionProvider;
  std::string m_sSymbol;

  double m_dblLast;
};
