#pragma once
#include "k:\data\projects\tradingsoftware\libtrading\symbol.h"
#include "TWS\EWrapper.h"

class CIBSymbol : public CSymbol {
  friend class CIBTWS;
public:
  CIBSymbol( const std::string sName, TickerId id  );
  virtual ~CIBSymbol(void);
  TickerId GetTickerId( void ) { return m_TickerId; };
protected:
  TickerId m_TickerId;
  bool m_bQuoteTradeWatchInProgress;
  bool m_bDepthWatchInProgress;

  bool m_bAskFound;
  bool m_bAskSizeFound;
  bool m_bBidFound;
  bool m_bBidSizeFound;

  bool m_bLastTimeStampFound;
  bool m_bLastFound;
  bool m_bLastSizeFound;

  int m_nAskSize;
  int m_nBidSize;
  int m_nLastSize;
  int m_nVolume;

  double m_dblAsk;
  double m_dblBid;
  double m_dblLast;

  double m_dblHigh;
  double m_dblLow;
  double m_dblClose;

  void AcceptTickPrice( TickType tickType, double price );
  void AcceptTickSize( TickType tickType, int size );
  void AcceptTickString( TickType tickType, const CString &value );

  void BuildQuote( void );
  void BuildTrade( void );

private:
};
