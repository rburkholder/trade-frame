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
#include <mutex>
#include <vector>
#include <string>
#include <functional>

#include "ChartEntryBase.h"

namespace ou { // One Unified

// TODO: migrate to cpp file with forward declaration
class ChartEntryCarrier { // used by ChartViewPort objects to chart data
public:
  //ChartDataViewCarrier( void );
  ChartEntryCarrier( size_t nChart, ChartEntryBase* );
  ChartEntryCarrier( const ChartEntryCarrier& );
  ChartEntryCarrier( ChartEntryCarrier&& );
  ~ChartEntryCarrier();
  ChartEntryCarrier& operator=( const ChartEntryCarrier& );
  void SetActualChartId( size_t ix ) { m_nActualChart = ix; };
  size_t GetLogicalChartId() const { return m_nLogicalChart; };
  size_t GetActualChartId() const { return m_nActualChart; };
  ChartEntryBase* GetChartEntry() { return m_pChartEntry; };
  const ChartEntryBase* GetChartEntry() const { return m_pChartEntry; };
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

  std::mutex m_mutex;

  using pChartDataView_t = std::shared_ptr<ChartDataView>;

  using ViewPort_t = ChartEntryTime::range_t;

  ChartDataView();
  virtual ~ChartDataView();

  static pChartDataView_t Factory() { return std::make_shared<ChartDataView>(); }

  void Add( size_t nChart, ChartEntryBase* pEntry );  // could try boost::fusion here?  some crtp stuff?
  void Remove( size_t nChart, ChartEntryBase* pEntry );
  void Clear();  // remove stuff in order to reuse.

  // can use not_a_date_time for one, the other, or both
  void SetViewPort( const ViewPort_t& );
  ViewPort_t GetViewPort() const;

  void SetNames( const std::string& sDescription, const std::string& sName ) {
    m_sDescription = sDescription;
    m_sName = sName;
  }
  const std::string& GetName() const { return m_sName; };
  const std::string& GetDescription() const { return m_sDescription; };

  ViewPort_t GetExtents();
  boost::posix_time::ptime GetExtentBegin();
  boost::posix_time::ptime GetExtentEnd();

  size_t GetChartCount() const { return m_mapCntChartIndexes.size(); };

  using fEachChartEntryCarrier_t = std::function<void( ChartEntryCarrier& )>;
  void EachChartEntryCarrier( fEachChartEntryCarrier_t&& f ) {
    for ( ChartEntryCarrier& carrier: m_vChartEntryCarrier ) {
      f( carrier );
    }
  }

  using fCursorDateTime_t = std::function<void( const boost::posix_time::ptime )>;
  void SetNotifyCursorDateTime( fCursorDateTime_t&& f ) { m_fCursorDateTime = std::move( f ); }
  void NotifyCursorDateTime( const boost::posix_time::ptime );

  void SetDebug( bool bDebug ) { m_bDebug = bDebug; }
  bool GetDebug() const { return m_bDebug; }

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

  bool m_bDebug;

  std::string m_sName;
  std::string m_sDescription;

  fCursorDateTime_t m_fCursorDateTime;

  boost::posix_time::ptime m_dtViewPortBegin;
  boost::posix_time::ptime m_dtViewPortEnd;

  using mapCntChartIndexes_t = std::map<size_t /* carrier nChart */, structChartMapping>;
  mapCntChartIndexes_t m_mapCntChartIndexes;  // how many of each carrier::m_nchart we have

  using vChartEntryCarrier_t = std::vector<ChartEntryCarrier>;
  vChartEntryCarrier_t m_vChartEntryCarrier;

  ViewPort_t GetExtents_NoLock() const;
  boost::posix_time::ptime GetExtentBegin_NoLock() const;
  boost::posix_time::ptime GetExtentEnd_NoLock() const;

  void UpdateActualChartId();

};

} // namespace ou

// http://www.parashift.com/c++-faq-lite/assignment-operators.html