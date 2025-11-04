/************************************************************************
 * Copyright(c) 2025, One Unified. All rights reserved.                 *
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

/*
 * File:    TimeSeriesModel.hpp
 * Author:  raymond@burkholder.net
 * Project: SP500
 * Created: November 1, 2025 19:12:28
 */

#pragma once

#include <vector>

#include <wx/grid.h>

#include "TFTimeSeries/TimeSeries.h"
#include "wx/generic/grid.h"

class TimeSeriesModel
: public wxGridTableBase {
public:

  TimeSeriesModel();
  virtual ~TimeSeriesModel();

  void Set( const ou::tf::Quotes*, const ou::tf::Trades* );

  void UpdateDateTime( const boost::posix_time::ptime );

  virtual void SetView( wxGrid * ) override;
  //virtual wxGrid* GetView() const override;

  protected:

  virtual int GetNumberRows() override;
  virtual int GetNumberCols() override;
  //virtual bool CanHaveAttributes() override;
  //virtual bool CanMeasureColUsingSameAttr( int col ) const override;

  virtual bool IsEmptyCell( int row, int col ) override;
  virtual wxString GetValue( int row, int col ) override;
  virtual void SetValue( int row, int col, const wxString &value ) override;

  virtual bool AppendRows( size_t numRows ) override;
  virtual bool InsertRows( size_t pos, size_t numRows ) override;
  virtual bool DeleteRows( size_t pos, size_t numRows ) override;

  virtual wxString GetColLabelValue( int col ) override;

  virtual wxGridCellAttr* GetAttr( int row, int col, wxGridCellAttr::wxAttrKind ) override;

private:

  enum EColId { dt, bid_vol, bid_prc, trd_vol, trd_prc, ask_vol, ask_prc, imbalance, _col_id_count };

  const ou::tf::Quotes* m_pQuotes;
  const ou::tf::Trades* m_pTrades;

  wxGrid* m_pGrid;

  size_t m_nRows;

  struct Row {
    wxString sDateTime;
    wxString sBidVol;
    wxString sBidPrice;
    wxString sTrdVol;
    wxString sTrdPrice;
    wxString sAskVol;
    wxString sAskPrice;
    wxString sImbalance;
    wxColour colourTrdPrice;
    wxColour colourImbalance;
    Row() {};
    Row( Row&& rhs )
    : sDateTime( std::move( rhs.sDateTime ) )
    , sBidVol( std::move( rhs.sBidVol ) )
    , sBidPrice( std::move( rhs.sBidPrice ) )
    , sTrdVol( std::move( rhs.sTrdVol ) )
    , sTrdPrice( std::move( rhs.sTrdPrice ) )
    , sAskVol( std::move( rhs.sAskVol ) )
    , sAskPrice( std::move( rhs.sAskPrice ) )
    , sImbalance( std::move( rhs.sImbalance ) )
    , colourTrdPrice( rhs.colourTrdPrice )
    , colourImbalance( rhs.colourImbalance )
    {}

  };

  using vRow_t = std::vector<Row>;
  vRow_t m_vRow;

};