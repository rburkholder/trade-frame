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

#include <map>
#include <vector>
#include <string>
#include <memory>

#include <OUCommon/Delegate.h>

#include "ChartEntryBase.h"

namespace ou { // One Unified

// TODO: migrate to cpp file with forward declaration
class ChartEntryCarrier { // used by ChartViewPort objects to chart data
public:
  //ChartDataViewCarrier( void );
  ChartEntryCarrier( size_t nChart, ChartEntryBase* pChartEntry );
  ChartEntryCarrier( const ChartEntryCarrier& carrier );
  ChartEntryCarrier( const ChartEntryCarrier&& carrier );
  ~ChartEntryCarrier( void );
  void SetActualChartId( size_t ix ) { m_nActualChart = ix; };
  size_t GetLogicalChartId( void ) const { return m_nLogicalChart; };
  size_t GetActualChartId( void ) const { return m_nActualChart; };
  ChartEntryBase* GetChartEntry( void ) { return m_pChartEntry; };
  const ChartEntryBase* GetChartEntry( void ) const { return m_pChartEntry; };
protected:
private:
  size_t m_nLogicalChart;  // as supplied by trading rules
  size_t m_nActualChart;   // as supplied by ChartDataView management (monotonically increasing)
  ChartEntryBase* m_pChartEntry;
};

// nChart:  0, 1 reserved:
//   0: main price chart
//   1: main volume chart

// ChartDataView contains the ChartEntries
//   to be viewed in a master chart viewport

class ChartDataView {
public:

  // TODO: won't need these once converted to lambda
  using const_iterator = std::vector<ChartEntryCarrier>::const_iterator;
  using       iterator = std::vector<ChartEntryCarrier>::iterator;

  using pChartDataView_t = std::shared_ptr<ChartDataView>;

  using ViewPort_t = ChartEntryTime::range_t;

  ChartDataView( void );
  virtual ~ChartDataView(void);

  void Add( size_t nChart, ChartEntryBase* pEntry );  // could try boost::fusion here?  some crtp stuff?
  void Remove( size_t nChart, ChartEntryBase* pEntry );
  void Clear( void );  // remove stuff in order to reuse.

  void SetChanged(void);
  bool GetChanged(void); // resets flag on read

  // can use not_a_date_time for one, the other, or both
  void SetViewPort( boost::posix_time::ptime dtBegin, boost::posix_time::ptime dtEnd );
  void SetViewPort( const ViewPort_t& );
  ViewPort_t GetViewPort() const;

  void SetNames( const std::string& sDescription, const std::string& sName ) {
    m_sDescription = sDescription;
    m_sName = sName;
  }
  const std::string& GetName( void ) const { return m_sName; };
  const std::string& GetDescription( void ) const { return m_sDescription; };

  ViewPort_t GetExtents() const;
  boost::posix_time::ptime GetExtentBegin() const;
  boost::posix_time::ptime GetExtentEnd() const;

  size_t GetChartCount( void ) const { return m_mapCntChartIndexes.size(); };

  // used by ChartMaster, maybe change to std::function iteration, or lambda callback
  iterator begin( void ) { return m_vChartEntryCarrier.begin(); };
  iterator end( void ) { return m_vChartEntryCarrier.end(); };

protected:

private:

  // chart# from ChartEntry may not be monotonically increasing, ixActualChartId is forced to be so
  struct structChartMapping {
    size_t ixActualChartId;
    size_t nChartEntries;  // number of ChartEntries for this Chart
    structChartMapping() : ixActualChartId( 0 ), nChartEntries( 0 ) {};
    explicit structChartMapping( const structChartMapping& rhs )
      : ixActualChartId( rhs.ixActualChartId ), nChartEntries( rhs.nChartEntries ) {};
    structChartMapping& operator=( const structChartMapping &rhs ) {
      ixActualChartId = rhs.ixActualChartId;
      nChartEntries = rhs.nChartEntries;
      return *this;
    };
  };

  bool m_bChanged;
  std::string m_sName;
  std::string m_sDescription;

  boost::posix_time::ptime m_dtViewPortBegin;
  boost::posix_time::ptime m_dtViewPortEnd;

  using mapCntChartIndexes_t = std::map<size_t /* carrier nChart */, structChartMapping>;
  mapCntChartIndexes_t m_mapCntChartIndexes;  // how many of each carrier::m_nchart we have

  using vChartEntryCarrier_t = std::vector<ChartEntryCarrier>;
  vChartEntryCarrier_t m_vChartEntryCarrier;

  void UpdateActualChartId();

};

} // namespace ou

// http://www.parashift.com/c++-faq-lite/assignment-operators.html