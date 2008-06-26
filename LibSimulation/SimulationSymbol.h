#pragma once

#include <string>

#include <TimeSeries.h>

#include "symbol.h"

class CSimulationSymbol: public CSymbol {
  friend class CSimulationProvider;
public:
  CSimulationSymbol(const std::string &sSymbol, const std::string &sDirectory );
  virtual ~CSimulationSymbol(void);
protected:
  void StartTradeWatch( void );
  void StopTradeWatch( void );
  void StartQuoteWatch( void );
  void StopQuoteWatch( void );
  void StartDepthWatch( void );
  void StopDepthWatch( void );

  void HandleTradeEvent( const CDatedDatum &datum ) {
    m_OnTrade( dynamic_cast<const CTrade &>( datum ) );  // use static_cast in non-debug mode
  };
  void HandleQuoteEvent( const CDatedDatum &datum ) {
    m_OnQuote( dynamic_cast<const CQuote &>( datum ) );  // use static_cast in non-debug mode
  }

  std::string m_sDirectory;

  CQuotes m_quotes;
  CTrades m_trades;
private:
};
