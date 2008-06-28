#include "StdAfx.h"
#include "SimulationSymbol.h"

//#include "HDF5DataManager.h"
//using namespace H5;

#include "HDF5TimeSeriesContainer.h"
#include "HDF5IterateGroups.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CSimulationSymbol::CSimulationSymbol( const std::string &sSymbol, const std::string &sDirectory) 
: CSymbol(sSymbol), m_sDirectory( sDirectory )
{
}

CSimulationSymbol::~CSimulationSymbol(void) {
}

void CSimulationSymbol::StartTradeWatch( void ) {
  std::string sPath( m_sDirectory + "/trades/" + m_sSymbolName );
  CHDF5TimeSeriesContainer<CTrade> tradeRepository( sPath );
  CHDF5TimeSeriesContainer<CTrade>::iterator begin, end;
  begin = tradeRepository.begin();
  end = tradeRepository.end();
  m_trades.Resize( end - begin );
  tradeRepository.Read( begin, end, &m_trades );
}

void CSimulationSymbol::StopTradeWatch( void ) {
}

void CSimulationSymbol::StartQuoteWatch( void ) {
  std::string sPath( m_sDirectory + "/quotes/" + m_sSymbolName );
  CHDF5TimeSeriesContainer<CQuote> quoteRepository( sPath );
  CHDF5TimeSeriesContainer<CQuote>::iterator begin, end;
  begin = quoteRepository.begin();
  end = quoteRepository.end();
  m_quotes.Resize( end - begin );
  quoteRepository.Read( begin, end, &m_quotes );
}

void CSimulationSymbol::StopQuoteWatch( void ) {
}

void CSimulationSymbol::StartDepthWatch( void ) {
}

void CSimulationSymbol::StopDepthWatch( void ) {
}

