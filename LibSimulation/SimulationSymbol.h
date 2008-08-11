#pragma once

#include <string>
#include <boost/detail/atomic_count.hpp>

#include <GuiThreadCrossing.h>
#include <TimeSeries.h>

#include "symbol.h"

class CSimulationSymbol: public CSymbol, CGuiThreadCrossing {
  DECLARE_DYNAMIC(CSimulationSymbol)
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

  void HandleTradeEvent( const CDatedDatum &datum );
  void HandleQuoteEvent( const CDatedDatum &datum );

  LRESULT OnCrossThreadArrivalQuoteEvent( WPARAM w, LPARAM l );
  LRESULT OnCrossThreadArrivalTradeEvent( WPARAM w, LPARAM l );

  std::string m_sDirectory;

  CQuotes m_quotes;
  CTrades m_trades;

  HANDLE m_hQuoteEventSignal;
  HANDLE m_hTradeEventSignal;

  CWinThread *m_pMainThread;
  //DWORD m_idMainThread;
private:
  DECLARE_MESSAGE_MAP()
};
