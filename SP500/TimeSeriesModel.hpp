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

#include <wx/grid.h>

#include "TFTimeSeries/TimeSeries.h"

class TimeSeriesModel
: public wxGridTableBase {
public:

  TimeSeriesModel( const ou::tf::Quotes&, const ou::tf::Trades& );
  virtual ~TimeSeriesModel();

  void UpdateDateTime( const boost::posix_time::ptime );

  //virtual void SetView( wxGrid *grid ) override;
  //virtual wxGrid* GetView() const override;

  protected:

  virtual int GetNumberRows() override;
  virtual int GetNumberCols() override;
  virtual bool CanHaveAttributes() override;
  virtual bool CanMeasureColUsingSameAttr( int col ) const override;

  virtual bool IsEmptyCell( int row, int col ) override;
  virtual wxString GetValue( int row, int col ) override;
  virtual void SetValue( int row, int col, const wxString &value ) override;

private:

  const ou::tf::Quotes& m_quotes;
  const ou::tf::Trades& m_trades;

};