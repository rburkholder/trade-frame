/************************************************************************
 * Copyright(c) 2013, One Unified. All rights reserved.                 *
 * email: info@oneunified.net                                           *
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

// Started 2013/10/26

#include <OUCharting/ChartDataView.h>

#include <OUCharting/ChartEntryBars.h>
#include <OUCharting/ChartEntryVolume.h>
#include <OUCharting/ChartEntryIndicator.h>

#include <TFHDF5TimeSeries/HDF5DataManager.h>
#include <TFHDF5TimeSeries/HDF5TimeSeriesContainer.h>

namespace ou { // One Unified
namespace tf { // TradeFrame

class ModelChartHdf5 {
public:

  ModelChartHdf5( void );
  virtual ~ModelChartHdf5(void);

  template<typename TS> // TS=timeseries
  void ChartTimeSeries( ou::tf::HDF5DataManager* pdm, ou::ChartDataView* pChartDataView, const std::string& sName, const std::string& sPath ) {

    pChartDataView->SetNames( sName, sPath );

    ou::tf::HDF5TimeSeriesContainer<typename TS::datum_t> tsRepository( *pdm, sPath );
    typename ou::tf::HDF5TimeSeriesContainer<typename TS::datum_t>::iterator begin, end;
    begin = tsRepository.begin();
    end = tsRepository.end();
    hsize_t cnt = end - begin;
    TS series;
    series.Resize( cnt );
    tsRepository.Read( begin, end, &series );

    AddChartEntries( pChartDataView, series );

//    m_winChart->RefreshRect( m_winChart->GetClientRect(), false );
  }

  template<typename TS> // TS=timeseries
  void ChartTimeSeries( ou::ChartDataView* pChartDataView, const TS& series, const std::string& sName, const std::string& sDescription ) {

    pChartDataView->SetNames( sName, sDescription );

//    ou::tf::HDF5TimeSeriesContainer<TS::datum_t> tsRepository( *pdm, sPath );
//    ou::tf::HDF5TimeSeriesContainer<TS::datum_t>::iterator begin, end;
//    begin = tsRepository.begin();
//    end = tsRepository.end();
//    hsize_t cnt = end - begin;
//    TS series;
//    series.Resize( cnt );
//    tsRepository.Read( begin, end, &series );

    AddChartEntries( pChartDataView, series );

//    m_winChart->RefreshRect( m_winChart->GetClientRect(), false );
  }

protected:
private:

  ou::ChartEntryIndicator m_ceQuoteUpper;
  ou::ChartEntryIndicator m_ceQuoteLower;
  ou::ChartEntryIndicator m_ceQuoteSpread;
  ou::ChartEntryIndicator m_ceTrade;
  ou::ChartEntryIndicator m_ceCallIV;
  ou::ChartEntryIndicator m_cePutIV;
  ou::ChartEntryIndicator m_ceImpVol;
  ou::ChartEntryIndicator m_ceDelta;
  ou::ChartEntryIndicator m_ceGamma;
  ou::ChartEntryIndicator m_ceTheta;
  ou::ChartEntryIndicator m_ceVega;
  ou::ChartEntryIndicator m_ceRho;
  ou::ChartEntryBars m_ceBars;
  ou::ChartEntryVolume m_ceVolume;
  ou::ChartEntryVolume m_ceVolumeUpper;
  ou::ChartEntryVolume m_ceVolumeLower;

  void AddChartEntries( ou::ChartDataView* pChartDataView, const ou::tf::Bars& bars );
  void AddChartEntries( ou::ChartDataView* pChartDataView, const ou::tf::Quotes& quotes );
  void AddChartEntries( ou::ChartDataView* pChartDataView, const ou::tf::Trades& trades );
  void AddChartEntries( ou::ChartDataView* pChartDataView, const ou::tf::PriceIVs& ivs );
  void AddChartEntries( ou::ChartDataView* pChartDataView, const ou::tf::Greeks& greeks );

};

} // namespace tf
} // namespace ou
