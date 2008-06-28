#pragma once
#include "TimeSeries.h"

#include <list>
using namespace std;

#include "FastDelegate.h"
using namespace fastdelegate;

#include "MergeDatedDAtumCarrier.h"

class CMergeDatedDatums {
public:
  CMergeDatedDatums(void);
  virtual ~CMergeDatedDatums(void);

  typedef FastDelegate1<const CDatedDatum &> OnDatumHandler;
  //typedef FastDelegate1<const CTrade &> OnTradeHandler;

  void Add( CTimeSeriesBase *pSeries, OnDatumHandler );
  //void Add( CTrades *pSeries, OnTradeHandler );
  void Run( void );
protected:
  list<CMergeCarrier *> m_lCarriers;
private:
};
