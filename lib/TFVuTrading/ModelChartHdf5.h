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

// 20170416 TODO: split into two modules or classes
//   generic chart definitions
//   hdf5 data retrieval for ChartHdf5

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

  struct Equities {
    const ou::tf::Quotes& quotes;
    const ou::tf::Trades& trades;
    Equities( const ou::tf::Quotes& quotes_, const ou::tf::Trades& trades_ )
      : quotes( quotes_ ), trades( trades_ ) {}
  };

  struct Options: public Equities {
    const ou::tf::Greeks& greeks;
    Options( const ou::tf::Quotes& quotes_, const ou::tf::Trades& trades_, const ou::tf::Greeks& greeks_ )
      : Equities( quotes_, trades_ ), greeks( greeks_ ) {}
  };

  // only usual timeseries can be used here
  template<typename TS> // TS=timeseries
  size_t ChartTimeSeries( ou::tf::HDF5DataManager* pdm, ou::ChartDataView* pChartDataView, const std::string& sName, const std::string& sPath ) {

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

    return cnt;

  }

  // Normal timeseries plus equities/options structs can be used here:
  template<typename TS> // TS=timeseries
  size_t ChartTimeSeries( ou::ChartDataView* pChartDataView, const TS& series, const std::string& sName, const std::string& sDescription ) {

    pChartDataView->SetNames( sName, sDescription );
    AddChartEntries( pChartDataView, series );

    return 0;

  }

  void HandleQuote( const ou::tf::Quote& quote );
  void HandleTrade( const ou::tf::Trade& trade );
  void HandleGreek( const ou::tf::Greek& greek );

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

  void Clear();

  void DefineChartBars( ou::ChartDataView* );
  void DefineChartQuotes( ou::ChartDataView* );
  void DefineChartTrades( ou::ChartDataView* );
  void DefineChartPriceIVs( ou::ChartDataView* );
  void DefineChartPriceIVExpirys( ou::ChartDataView* );
  void DefineChartGreeks( ou::ChartDataView* );
  void DefineChartEquities( ou::ChartDataView* );
  void DefineChartOptions( ou::ChartDataView* );
  void DefineChartDepthsByMM( ou::ChartDataView* );
  void DefineChartDepthsByOrder( ou::ChartDataView* );

  void AddChartEntries( ou::ChartDataView*, const ou::tf::Bars& );
  void AddChartEntries( ou::ChartDataView*, const ou::tf::Quotes& );
  void AddChartEntries( ou::ChartDataView*, const ou::tf::Trades& );
  void AddChartEntries( ou::ChartDataView*, const ou::tf::PriceIVs& );
  void AddChartEntries( ou::ChartDataView*, const ou::tf::PriceIVExpirys& );
  void AddChartEntries( ou::ChartDataView*, const ou::tf::Greeks& );
  void AddChartEntries( ou::ChartDataView*, const Equities& );
  void AddChartEntries( ou::ChartDataView*, const Options& );
  void AddChartEntries( ou::ChartDataView*, const ou::tf::DepthsByMM& );
  void AddChartEntries( ou::ChartDataView*, const ou::tf::DepthsByOrder& );

};

} // namespace tf
} // namespace ou
