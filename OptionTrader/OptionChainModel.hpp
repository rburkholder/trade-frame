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
 * File:    OptionChainModel.hpp
 * Author:  raymond@burkholder.net
 * Project: OptionTrader
 * Created: July 18, 2025 11:30:59
 */

#pragma once

#include <wx/grid.h>

#include "Common.hpp"

class OptionChainModel_impl;

class OptionChainModel
: public wxGridTableBase
{
  friend OptionChainModel_impl;
public:

  using pInstrument_t = ou::tf::Instrument::pInstrument_t;
  using pOption_t = ou::tf::option::Option::pOption_t;

  using fBuildOption_t = std::function<pOption_t( pInstrument_t )>;
  using fOptionEngineAction_t = std::function<void( pOption_t )>;

  OptionChainModel(
    mapChains_t::value_type&
  , fBuildOption_t&
  , fOptionEngineAction_t&& fOptionEngineStart
  , fOptionEngineAction_t&& fOptionEngineStop
  );
  ~OptionChainModel();

  void HandleTimer( int top_row, int visible_row_count );

  int ClosestStrike( double ) const;

  void OptionSelected( int row, int col );

protected:
private:

  using pOptionChainModel_impl_t = std::unique_ptr<OptionChainModel_impl>;
  pOptionChainModel_impl_t m_pOptionChainModel_impl;

  virtual void SetView ( wxGrid* ) override;

  virtual int GetNumberRows() override;
  virtual int GetNumberCols() override;

  virtual bool IsEmptyCell( int row, int col ) override;

  virtual wxString GetValue( int row, int col	) override;
  virtual void SetValue( int row, int col, const wxString& ) override;

  virtual void Clear() override;

  virtual bool InsertRows( size_t pos=0, size_t numRows=1 ) override;
  virtual bool AppendRows( size_t numRows=1 ) override;

  virtual wxGridCellAttr* GetAttr ( int row, int col, wxGridCellAttr::wxAttrKind kind ) override;
  virtual wxString GetColLabelValue( int col ) override;

};