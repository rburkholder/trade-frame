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

#include <algorithm>

#include "ChartDataView.h"

namespace ou { // One Unified

//
// ChartEntryCarrier
//

ChartEntryCarrier::ChartEntryCarrier( size_t nChart, ChartEntryBase* pChartEntry )
: m_nLogicalChart( nChart ), m_nActualChart( 0 ), m_pChartEntry( pChartEntry )
{
}

ChartEntryCarrier::ChartEntryCarrier( const ChartEntryCarrier& rhs )
: m_nLogicalChart( rhs.m_nLogicalChart ),
  m_nActualChart( rhs.m_nActualChart ),
  m_pChartEntry( rhs.m_pChartEntry )
{
}

ChartEntryCarrier::ChartEntryCarrier( const ChartEntryCarrier&& rhs )
: m_nLogicalChart( rhs.m_nLogicalChart ),
  m_nActualChart( rhs.m_nActualChart ),
  m_pChartEntry( std::move( rhs.m_pChartEntry ) )
{
}

ChartEntryCarrier::~ChartEntryCarrier() {
  m_nLogicalChart = 0;
  m_nActualChart = 0;
  m_pChartEntry = 0;
}

//
// ChartDataView
//

ChartDataView::ChartDataView()
: m_dtViewPortBegin( boost::posix_time::not_a_date_time )
, m_dtViewPortEnd( boost::posix_time::not_a_date_time )
, m_fCursorDateTime( nullptr )
{
}

ChartDataView::~ChartDataView() {
//  assert( m_bClosed );
  Clear();
}

void ChartDataView::UpdateActualChartId() {
  size_t ix = 0; // set ixActualChartId's to monotonically increasing
  std::for_each(
    m_mapCntChartIndexes.begin(), m_mapCntChartIndexes.end(),
    [this, ix](mapCntChartIndexes_t::value_type& vt) mutable {
      vt.second.ixActualChartId = ix++;
    });

  std::for_each( // update ActualChartId's in all the carriers.
    m_vChartEntryCarrier.begin(), m_vChartEntryCarrier.end(),
    [this](ChartEntryCarrier& cec){
      mapCntChartIndexes_t::iterator iter = m_mapCntChartIndexes.find( cec.GetLogicalChartId() );
      // TODO: need to work on this when shutting down: ced.m_nLogicalChart not found map
      //   maybe faulty logic in Remove
      //assert( m_mapCntChartIndexes.end() != iter );
      if ( m_mapCntChartIndexes.end() != iter ) {
        cec.SetActualChartId( iter->second.ixActualChartId );
      }
    });
}

void ChartDataView::Add( size_t nChart, ChartEntryBase* pEntry ) {
  std::scoped_lock<std::mutex> lock( m_mutex );

  m_vChartEntryCarrier.push_back( std::move( ChartEntryCarrier( nChart, pEntry ) ) );

  mapCntChartIndexes_t::iterator iter1 = m_mapCntChartIndexes.find( nChart );
  if ( m_mapCntChartIndexes.end() == iter1 ) {
    // need to add new mapping
    structChartMapping mapping;
    mapping.nChartEntries = 1;
    m_mapCntChartIndexes[ nChart ] = mapping;
    UpdateActualChartId();
  }
  else {
    (iter1->second.nChartEntries)++;
    m_vChartEntryCarrier.back().SetActualChartId( iter1->second.ixActualChartId );
  }
}

void ChartDataView::Remove( size_t nChart, ChartEntryBase* pEntry ) {
  std::scoped_lock<std::mutex> lock( m_mutex );

  vChartEntryCarrier_t::iterator iterChartEntryCarrier = std::find_if(
    m_vChartEntryCarrier.begin(), m_vChartEntryCarrier.end(),
    [nChart,pEntry](ChartEntryCarrier& cec)->bool{
      return ( ( cec.GetLogicalChartId() == nChart )
            && ( cec.GetChartEntry() == pEntry ) );
    });
  if ( m_vChartEntryCarrier.end() == iterChartEntryCarrier ) throw std::runtime_error( "could not find ChartEntryCarrier" );
  mapCntChartIndexes_t::iterator iterChartLookup = m_mapCntChartIndexes.find( iterChartEntryCarrier->GetLogicalChartId() );
  if ( m_mapCntChartIndexes.end() == iterChartLookup ) throw std::runtime_error( "count not find ChartIndexes" );
  assert( 0 < iterChartLookup->second.nChartEntries );
  iterChartLookup->second.nChartEntries--;
  if ( 0 == iterChartLookup->second.nChartEntries ) {
    m_mapCntChartIndexes.erase( iterChartLookup );
    UpdateActualChartId();
  }
}

void ChartDataView::Clear() {
  std::scoped_lock<std::mutex> lock( m_mutex );
  m_mapCntChartIndexes.clear();
  m_vChartEntryCarrier.clear();
}

//void ChartDataView::Close() {
//  assert( !m_bClosed );
//  OnClosing( this );
//  m_bClosed = true;
//}

void ChartDataView::SetViewPort( boost::posix_time::ptime dtBegin, boost::posix_time::ptime dtEnd ) {
  std::scoped_lock<std::mutex> lock( m_mutex );

  m_dtViewPortBegin = dtBegin;
  m_dtViewPortEnd = dtEnd;
  // need to change DataArrays in each entry
  for ( vChartEntryCarrier_t::iterator iter = m_vChartEntryCarrier.begin(); m_vChartEntryCarrier.end() != iter; ++iter ) {
    ChartEntryTime* p( dynamic_cast<ChartEntryTime*>( iter->GetChartEntry() ) );
    if ( nullptr != p ) {
      p->SetViewPort( dtBegin, dtEnd );
    }
  }
}

void ChartDataView::SetViewPort( const ViewPort_t& vp ) {
  SetViewPort( vp.dtBegin, vp.dtEnd );
}

ChartDataView::ViewPort_t ChartDataView::GetViewPort() const {
  return ViewPort_t( m_dtViewPortBegin, m_dtViewPortEnd );
}

ChartDataView::ViewPort_t ChartDataView::GetExtents() {
  std::scoped_lock<std::mutex> lock( m_mutex );
  return GetExtents_NoLock();
}

ChartDataView::ViewPort_t ChartDataView::GetExtents_NoLock() const {

  ViewPort_t view;
  std::for_each(
    m_vChartEntryCarrier.begin(), m_vChartEntryCarrier.end(),
    [&view]( const ChartEntryCarrier& cec ){
      try {
        const ChartEntryTime* p = dynamic_cast<const ChartEntryTime*>( cec.GetChartEntry() );
        if ( p ) {
          ViewPort_t extent = p->GetExtents();

          if ( boost::posix_time::not_a_date_time != view.dtBegin ) { // will default with extent assignment
            if ( boost::posix_time::not_a_date_time != extent.dtBegin ) { // no default assignment
              if ( extent.dtBegin < view.dtBegin ) {
                view.dtBegin = extent.dtBegin;
              }
            }
            else {}
          }
          else {
            view.dtBegin = extent.dtBegin;
          }

          if ( boost::posix_time::not_a_date_time != view.dtEnd ) { // will default with extent assignment
            if ( boost::posix_time::not_a_date_time != extent.dtEnd ) { // no default assignment
              if ( extent.dtEnd > view.dtEnd ) {
                view.dtEnd = extent.dtEnd;
              }
            }
            else {}
          }
          else {
            view.dtEnd = extent.dtEnd;
          }
        }

      }
      catch ( const std::bad_cast& ) {
        // just ignore classes without ChartEntryTime
      }
      catch ( ... ) {
        assert( false );
      }
    });
  return view;
}

boost::posix_time::ptime ChartDataView::GetExtentBegin() {
  std::scoped_lock<std::mutex> lock( m_mutex );
  return GetExtentBegin_NoLock();
}

boost::posix_time::ptime ChartDataView::GetExtentBegin_NoLock() const {
  boost::posix_time::ptime begin { boost::posix_time::not_a_date_time };
  std::for_each(
    m_vChartEntryCarrier.begin(), m_vChartEntryCarrier.end(),
    [&begin](const ChartEntryCarrier& cec ){
      try {
        boost::posix_time::ptime extent = dynamic_cast<const ChartEntryTime*>( cec.GetChartEntry() )->GetExtentBegin();

        if ( boost::posix_time::not_a_date_time != begin ) {
          if ( boost::posix_time::not_a_date_time != extent ) {
            if ( extent < begin ) {
              begin = extent;
            }
          }
        }
        else {
          begin = extent;
        }

      }
      catch (... ) {
        // just ignore classes without ChartEntryTime
      }
    });
  return begin;
}

boost::posix_time::ptime ChartDataView::GetExtentEnd() {
  std::scoped_lock<std::mutex> lock( m_mutex );
  return GetExtentEnd_NoLock();
}
boost::posix_time::ptime ChartDataView::GetExtentEnd_NoLock() const {
  boost::posix_time::ptime end { boost::posix_time::not_a_date_time };
  std::for_each(
    m_vChartEntryCarrier.begin(), m_vChartEntryCarrier.end(),
    [&end](const ChartEntryCarrier& cec ){
      try {
        boost::posix_time::ptime extent = dynamic_cast<const ChartEntryTime*>( cec.GetChartEntry() )->GetExtentEnd();

        if ( boost::posix_time::not_a_date_time != end ) {
          if ( boost::posix_time::not_a_date_time != extent ) {
            if ( extent > end ) {
              end = extent;
            }
          }
        }
        else {
          end = extent;
        }

      }
      catch (... ) {
        // just ignore classes without ChartEntryTime
      }
    });
  return end;
}

void ChartDataView::NotifyCursorDateTime( const boost::posix_time::ptime dt ) {
  if ( m_fCursorDateTime ) m_fCursorDateTime( dt );
}

} // namespace ou
