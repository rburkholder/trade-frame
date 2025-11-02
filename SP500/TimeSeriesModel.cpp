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
 * File:    TimeSeriesModel.cpp
 * Author:  raymond@burkholder.net
 * Project: SP500
 * Created: November 1, 2025 19:12:28
 */

#include "TimeSeriesModel.hpp"

TimeSeriesModel::TimeSeriesModel( const ou::tf::Quotes& quotes, const ou::tf::Trades& trades )
: wxGridTableBase()
, m_quotes( quotes )
, m_trades( trades )
{
}

TimeSeriesModel::~TimeSeriesModel() {
}

int TimeSeriesModel::GetNumberRows() {
  return 0;
}

int TimeSeriesModel::GetNumberCols() {
  return 0;
}

void TimeSeriesModel::SetView( wxGrid *grid ) {
}

//wxGrid* TimeSeriesModel::GetView() const {
//}

bool TimeSeriesModel::CanHaveAttributes() {
  return false;
}

bool TimeSeriesModel::CanMeasureColUsingSameAttr( int col ) const {
  return false;
}

bool TimeSeriesModel::IsEmptyCell( int row, int col ) {
  return true;
}

wxString TimeSeriesModel::GetValue( int row, int col ) {
  return wxString( "test" );
}

void TimeSeriesModel::SetValue( int row, int col, const wxString &value ) {
}
