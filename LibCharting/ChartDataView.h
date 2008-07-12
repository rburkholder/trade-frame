#pragma once

#include <vector>
#include <string>

#include <Delegate.h>

#include "ChartEntryBase.h"

// CChartDataView contains the CChartEntries and related sub-chart 
//   to be viewed in a master chart viewport 

class CChartDataViewCarrier { // used by CChartViewPort objects to chart data
public:
  CChartDataViewCarrier( void );
  CChartDataViewCarrier( size_t nChart, CChartEntryBaseWithTime *pChartEntry );
  CChartDataViewCarrier( const CChartDataViewCarrier &carrier );
  ~CChartDataViewCarrier( void );
  size_t GetChartId( void ) { return m_nChart; };
  CChartEntryBaseWithTime *GetChartEntry( void ) { return m_pChartEntry; };
protected:
  size_t m_nChart;
  CChartEntryBaseWithTime *m_pChartEntry;
private:
};

// nChart:  0, 1 reserved:
//   0: main price chart
//   1: main volume chart

class CChartDataView {
public:
  CChartDataView( const std::string &sStrategy, const std::string &sName );
  ~CChartDataView(void);
  void Add( size_t nChart, CChartEntryBaseWithTime *pChartEntry );
  typedef std::vector<CChartDataViewCarrier>::const_iterator const_iterator;
  const_iterator begin( void ) { return m_vChartDataViewEntry.begin(); };
  const_iterator end( void ) { return m_vChartDataViewEntry.end(); };
  const std::string &GetStrategy( void ) { return m_sStrategy; };
  const std::string &GetName( void ) { return m_sName; };
  Delegate<CChartDataView *> OnClosing;
  void Close( void ); // call before destruction so can be removed from tree view and view port properly
protected:
  std::vector<CChartDataViewCarrier> m_vChartDataViewEntry;
  bool m_bClosed;
  std::string m_sStrategy;
  std::string m_sName;
private:
};
