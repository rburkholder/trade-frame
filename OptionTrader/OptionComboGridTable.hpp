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
 * File:    OptionComboGridTable.hpp
 * Author:  raymond@burkholder.net
 * Project: OptionTrader
 * Created: December 29 2025 22:23:26
 */

#include <wx/grid.h>

namespace ou { // One Unified
namespace tf { // TradeFrame

class OptionComboGridTable
: public wxGridTableBase
{
public:
  OptionComboGridTable();
  ~OptionComboGridTable();

  virtual int GetNumberRows() override;
  virtual int GetNumberCols() override;
  virtual void SetView( wxGrid* ) override;
  virtual wxGrid* GetView() const override;
  virtual wxString GetValue( int row, int col ) override;
  virtual void SetValue( int row, int col, const wxString& ) override;

protected:
private:
};

} // namespace tf
} // namespace ou
