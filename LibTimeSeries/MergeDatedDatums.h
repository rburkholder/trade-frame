#pragma once
#include "TimeSeries.h"

#include <vector>

#include "FastDelegate.h"
using namespace fastdelegate;

#include "MergeDatedDAtumCarrier.h"

class CMergeDatedDatums {
public:
  CMergeDatedDatums(void);
  virtual ~CMergeDatedDatums(void);

  typedef FastDelegate1<const CDatedDatum &> OnDatumHandler;
  //typedef FastDelegate1<const CTrade &> OnTradeHandler;

  void Add( CTimeSeries<CQuote> *pSeries, OnDatumHandler );
  void Add( CTimeSeries<CTrade> *pSeries, OnDatumHandler );
  void Add( CTimeSeries<CBar> *pSeries, OnDatumHandler );
  void Add( CTimeSeries<CMarketDepth> *pSeries, OnDatumHandler );
  void Run( void );
protected:
  std::vector<CMergeCarrierBase *> m_vCarriers;
private:
};
