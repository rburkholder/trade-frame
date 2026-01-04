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
 * File:    OptionComboGridTable.cpp
 * Author:  raymond@burkholder.net
 * Project: OptionTrader
 * Created: December 29 2025 22:23:26
 */

#include "OptionComboGridTable.hpp"

namespace ou { // One Unified
namespace tf { // TradeFrame

OptionComboGridTable::OptionComboGridTable()
:wxGridTableBase()
{
}

OptionComboGridTable::~OptionComboGridTable() {
}

int OptionComboGridTable::GetNumberRows() { return 0; }
int OptionComboGridTable::GetNumberCols() { return 0; }
void OptionComboGridTable::SetView( wxGrid* ) {}
wxGrid* OptionComboGridTable::GetView() const { return nullptr; }
wxString OptionComboGridTable::GetValue( int row, int col ) { return "not implemented"; }
void OptionComboGridTable::SetValue( int row, int col, const wxString& ) {}

} // namespace tf
} // namespace ou
