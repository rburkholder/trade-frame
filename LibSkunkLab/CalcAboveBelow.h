#pragma once

// From HDF5 file, calculates statistics for price range above open and below open

#include "ProviderInterface.h"
#include "Portfolio.h"
#include "Symbol.h"
#include "Instrument.h"

#include <string>
#include <map>

class CAboveBelowInstrument {
public:
  CAboveBelowInstrument( void ) {}; 
  ~CAboveBelowInstrument( void ) {}; 
protected:
private:
};

class CAboveBelowUnderlying : public CAboveBelowInstrument {
public:
  CAboveBelowUnderlying( void ) {};
  ~CAboveBelowUnderlying( void ) {};
protected:
private:
};

class CAboveBelowOption : public CAboveBelowInstrument {
public:
  CAboveBelowOption( void ) {};
  ~CAboveBelowOption( void ) {};
protected:
private:
};

class CCalcAboveBelow {
public:
  CCalcAboveBelow( CInstrument *pInstrument, CProviderInterface *pDataProvider, CProviderInterface *pExecutionProvider );
  ~CCalcAboveBelow(void);
  void Start( void );
  void Stop( void );
protected:
  void HandleUpdatePortfolioRecord( CPortfolio::UpdatePortfolioRecord_t );
  void HandleTrade( const CTrade &trade );
private:
  CInstrument* m_pInstrument;
  CProviderInterface* m_pDataProvider;
  CProviderInterface* m_pExecutionProvider;

  //std::string m_sSymbolName;

  double m_dblLast;

  typedef std::map<std::string, CAboveBelowInstrument*> map_instruments_t;
  map_instruments_t m_mapInstruments;

};
