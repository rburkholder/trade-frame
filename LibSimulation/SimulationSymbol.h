#pragma once

#include <string>
#include <boost/detail/atomic_count.hpp>

//#include <GuiThreadCrossing.h>
#include <TimeSeries.h>
#include "SimulateOrderExecution.h"

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

  //virtual bool AddTradeHandler( CSymbol::tradehandler_t ); 
  //virtual bool RemoveTradeHandler( CSymbol::tradehandler_t );

  void HandleTradeEvent( const CDatedDatum &datum );
  void HandleQuoteEvent( const CDatedDatum &datum );

  std::string m_sDirectory;

  CQuotes m_quotes;
  CTrades m_trades;

  CSimulateOrderExecution m_simExec;
private:
};
