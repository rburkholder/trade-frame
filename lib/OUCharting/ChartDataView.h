/************************************************************************
 * Copyright(c) 2010, One Unified. All rights reserved.                 *
 *                                                                      *
 * This file is provided as is WITHOUT ANY WARRANTY                     *
 *  without even the implied warranty of                                *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.                *
 *                                                                      *
 * This software may not be used nor distributed without proper license *
 * agreement.                                                           *
 *                                                                      *
 * See the file LICENSE.txt for redistribution information.             *
 ************************************************************************/

#pragma once

#include <vector>
#include <string>
#include <map>

#include <OUCommon/Delegate.h>

#include "ChartEntryBase.h"

namespace ou { // One Unified
namespace local {

// ChartDataView contains the CChartEntries and related sub-chart
//   to be viewed in a master chart viewport

class ChartDataViewCarrier { // used by ChartViewPort objects to chart data
public:
  //ChartDataViewCarrier( void );
  ChartDataViewCarrier( size_t nChart, ChartEntryBase* pChartEntry );
  ChartDataViewCarrier( const ChartDataViewCarrier& carrier );
  ~ChartDataViewCarrier( void );
  size_t GetLogicalChartId( void ) const { return m_nLogicalChart; };
  void SetActualChartId( size_t ix ) { m_nActualChart = ix; };
  size_t GetActualChartId( void ) const { return m_nActualChart; };
  ChartEntryBase* GetChartEntry( void ) { return m_pChartEntry; };
protected:
  size_t m_nLogicalChart;  // as supplied by trading rules
  size_t m_nActualChart;   // as supplied by CChartDataView management
  ChartEntryBase* m_pChartEntry;
private:
};

} // local

// nChart:  0, 1 reserved:
//   0: main price chart
//   1: main volume chart

class ChartDataView {
public:

  typedef std::vector<local::ChartDataViewCarrier>::const_iterator const_iterator;
  typedef std::vector<local::ChartDataViewCarrier>::iterator iterator;

  ChartDataView( void );
  //ChartDataView( const std::string &sStrategy, const std::string &sName );
  virtual ~ChartDataView(void);

  void Add( size_t nChart, ChartEntryBase* pEntry );  // could try boost::fusion here?  some crtp stuff?
  iterator begin( void ) { return m_vChartDataViewCarrier.begin(); };
  iterator end( void ) { return m_vChartDataViewCarrier.end(); };
  const std::string& GetName( void ) const { return m_sName; };
  const std::string& GetDescription( void ) const { return m_sDescription; };
  void Clear( void );  // remove stuff in order to reuse.
  size_t GetChartCount( void ) const{ return m_mapCntChartIndexes.size(); };
  void SetChanged(void);
  bool GetChanged(void);

  // should reprocess m_vChartDataViewEntry when these are called
  void SetThreadSafe( bool bThreadSafe );
  bool GetThreadSafe( void ) const  { return m_bThreadSafe; }

  // can use not_a_date_time for one, the other, or both
  void SetViewPort( boost::posix_time::ptime dtBegin, boost::posix_time::ptime dtEnd );
  void SetNames( const std::string& sDescription, const std::string& sName ) {
    m_sDescription = sDescription;
    m_sName = sName;
  }

protected:

private:

  struct structChartMapping {
    size_t ixActualChartId;  // actual chart index
    size_t nCharts;  // number of charts at this index
    explicit structChartMapping( void ) : ixActualChartId( 0 ), nCharts( 0 ) {};
    explicit structChartMapping( const structChartMapping& obj )
      : ixActualChartId( obj.ixActualChartId ), nCharts( obj.nCharts ) {};
    structChartMapping& operator=( const structChartMapping &obj ) {
      ixActualChartId = obj.ixActualChartId;
      nCharts = obj.nCharts;
      return *this;
    };
  };

  //typedef std::map<size_t /* carrier nChart */, structChartMapping> mapCntChartIndexes_t;
  typedef std::map<boost::uint64_t /* carrier nChart */, structChartMapping> mapCntChartIndexes_t;

  typedef std::vector<local::ChartDataViewCarrier> vChartDataViewCarrier_t;

  bool m_bChanged;
  bool m_bThreadSafe;   // propagated into ChartEntries for value append operations across thread boundaries
  std::string m_sName;
  std::string m_sDescription;

  boost::posix_time::ptime m_dtViewPortBegin;
  boost::posix_time::ptime m_dtViewPortEnd;

  mapCntChartIndexes_t m_mapCntChartIndexes;  // how many of each carrier::m_nchart we have

  vChartDataViewCarrier_t m_vChartDataViewCarrier;

};

} // namespace ou

// http://www.parashift.com/c++-faq-lite/assignment-operators.html