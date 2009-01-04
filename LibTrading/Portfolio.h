#pragma once

#include "Instrument.h"
#include "Delegate.h"
#include "PortfolioRecord.h"

#include <string>

class CPortfolio {
public:
  CPortfolio( const std::string &sPortfolioName );
  ~CPortfolio(void);

  struct structUpdatePortfolioRecord {
    CInstrument *pInstrument;
    int nPosition;
    double dblPrice;
    double dblAverageCost;
    structUpdatePortfolioRecord( CInstrument *pInstrument_, int nPosition_, double dblPrice_, double dblAverageCost_ )
      : pInstrument( pInstrument_ ), nPosition( nPosition_ ), dblPrice( dblPrice_ ), dblAverageCost( dblAverageCost_ ) {};
  };

  typedef const structUpdatePortfolioRecord &UpdatePortfolioRecord_t;
  typedef Delegate<UpdatePortfolioRecord_t>::OnMessageHandler UpdatePortfolioRecordHandler_t;

  void AddRecord( const std::string &sName, CInstrument *pInstrument );
  void DeleteRecord( const std::string &sName );
  CPortfolioRecord *GetRecord( const std::string &sName );

  // need an on change event so delta can be recalculated on value or record addition (quote, trade, portfolio record, order, execution )


protected:
  std::string m_sPortfolioName;

  // has series of orders, COrder
  // has series of executions, CExecution
private:
};
