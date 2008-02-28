#pragma once
#include "TimeSeries.h"

#include <list>
using namespace std;

#include "FastDelegate.h"
using namespace fastdelegate;

class CMergeCarrier {
public:
  typedef FastDelegate1<const CDatedDatum &> OnDatumHandler;

  CMergeCarrier( CTimeSeries<CDatedDatum> *pSeries, OnDatumHandler function );
  virtual ~CMergeCarrier( void );
  void ProcessDatum( void );
  ptime m_dt;  // datetime of datum to be merged (used in comparison)
  CTimeSeries<CDatedDatum> *m_pSeries;  // series from which a datum is to be merged to output
  CDatedDatum *m_pDatum;
protected:
  OnDatumHandler OnDatum;
private:
};

class CMergeDatedDatums {
public:
  CMergeDatedDatums(void);
  virtual ~CMergeDatedDatums(void);

  typedef FastDelegate1<const CDatedDatum &> OnDatumHandler;

  void Add( CTimeSeries<CDatedDatum> *pSeries, OnDatumHandler );
  void Run( void );
protected:
  list<CMergeCarrier *> m_lCarriers;
private:
};
