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

#include <vector>

#include <wx/grid.h>

#include <TFVuTrading/ModelCell_macros.h>

#include "Common.hpp"

class OptionChainView;

class OptionChainModel
: public wxGridTableBase
{
  friend OptionChainView;
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

  virtual void SetView ( wxGrid *grid ) override;
  virtual wxGrid* GetView() const override;

protected:
private:

  // for column 2, use wxALIGN_LEFT, wxALIGN_CENTRE or wxALIGN_RIGHT
  #define GRID_ARRAY_PARAM_COUNT 5
  #define GRID_ARRAY_COL_COUNT 13
  #define GRID_ARRAY \
    (GRID_ARRAY_COL_COUNT,  \
      ( /* Col 0,         1,             2,        3,      4,          */ \
        ( col_CallOi,    "c oi",   wxALIGN_RIGHT,   50, ModelCellInt    ), \
        ( col_CallIV,    "c iv",   wxALIGN_RIGHT,   50, ModelCellDouble ), \
        ( col_CallBid,   "c bid",  wxALIGN_RIGHT,   50, ModelCellDouble ), \
        ( col_CallAsk,   "c ask",  wxALIGN_RIGHT,  120, ModelCellDouble ), \
        ( col_CallGamma, "c gma",  wxALIGN_RIGHT,   50, ModelCellDouble ), \
        ( col_CallDelta, "c dlt",  wxALIGN_RIGHT,   50, ModelCellDouble ), \
        ( col_Strike,    "strike", wxALIGN_RIGHT,   50, ModelCellDouble ), \
        ( col_PutDelta,  "p dlt",  wxALIGN_RIGHT,   50, ModelCellDouble ), \
        ( col_PutGamma,  "p gma",  wxALIGN_RIGHT,   50, ModelCellDouble ), \
        ( col_PutBid,    "p bid",  wxALIGN_RIGHT,   50, ModelCellDouble ), \
        ( col_PutAsk,    "p ask",  wxALIGN_RIGHT,  120, ModelCellDouble ), \
        ( col_PutIV,     "p iv",   wxALIGN_RIGHT,   50, ModelCellDouble ), \
        ( col_PutOi,     "p oi",   wxALIGN_RIGHT,   50, ModelCellInt    ), \
        ) \
      )

  enum EChainColums {
    BOOST_PP_REPEAT(GRID_ARRAY_COL_COUNT,GRID_EXTRACT_ENUM_LIST,0)
  };

  mapChains_t::value_type& m_vt;

  fBuildOption_t& m_fBuildOption;
  fOptionEngineAction_t m_fOptionEngineStart;
  fOptionEngineAction_t m_fOptionEngineStop;

  struct Strike {
    size_t nWatching;
    bool bUpdated;  // used by timed scan to generate event to control
    double strike;
    chain_t::strike_t& options;
    fBuildOption_t& fBuildOption;
    fOptionEngineAction_t& fOptionEngineStart;
    fOptionEngineAction_t& fOptionEngineStop;

    Strike(
      double strike_, chain_t::strike_t& options_
    , fBuildOption_t& fBuildOption_
    , fOptionEngineAction_t& fOptionEngineStart_
    , fOptionEngineAction_t& fOptionEngineStop_
    )
    : strike( strike_ ), options( options_ ), fBuildOption( fBuildOption_ )
    , nWatching( 0 ), bUpdated( false )
    , fOptionEngineStart( fOptionEngineStart_ )
    , fOptionEngineStop( fOptionEngineStop_ )
    {}

    Strike( const Strike& rhs )
    : strike( rhs.strike ), options( rhs.options ), fBuildOption( rhs.fBuildOption )
    , nWatching( rhs.nWatching ), bUpdated( rhs.bUpdated )
    , fOptionEngineStart( rhs.fOptionEngineStart )
    , fOptionEngineStop( rhs.fOptionEngineStop )
    {}

    bool IsWatching() const { return 0 < nWatching; }

    void Start() {
      if ( 0 == nWatching ) {
        if ( options.call.pInstrument ) {
          if ( !options.call.pOption ) {
            options.call.pOption = fBuildOption( options.call.pInstrument );
          }
          pOption_t& pOption( options.call.pOption );
          pOption->OnQuote.Add( MakeDelegate( this, &Strike::HandleQuote ) );
          //options.call.pOption->OnTrade.Add( MakeDelegate( this, &Strike::HandleTrade ) );
          pOption->StartWatch();
          fOptionEngineStart( pOption );
        }
        if ( options.put.pInstrument ) {
          if ( !options.put.pOption ) {
            options.put.pOption = fBuildOption( options.put.pInstrument );
          }
          pOption_t& pOption( options.put.pOption );
          pOption->OnQuote.Add( MakeDelegate( this, &Strike::HandleQuote ) );
          //options.put.pOption->OnTrade.Add( MakeDelegate( this, &Strike::HandleTrade ) );
          pOption->StartWatch();
          fOptionEngineStart( pOption );
        }
      }
      ++nWatching;
    }

    void HandleQuote( const ou::tf::Quote& quote ) {
      bUpdated = true; // todo: confirm with change in value of bid/ask
    }

    void HandleTrade( const ou::tf::Trade& trade ) {
    }

    bool Updated() {
      bool b( bUpdated );
      bUpdated = false;
      return b;
    }

    void Stop() {
      if ( 0 < nWatching ) {
        if ( 1 == nWatching ) {
          bUpdated = false;
          if ( options.call.pInstrument ) {
            if ( options.call.pOption ) {
              pOption_t& pOption( options.call.pOption );
              fOptionEngineStop( pOption );
              pOption->StopWatch();
              pOption->OnQuote.Remove( MakeDelegate( this, &Strike::HandleQuote ) );
              //options.call.pOption->OnTrade.Remove( MakeDelegate( this, &Strike::HandleTrade ) );
            }
          }
          if ( options.put.pInstrument ) {
            if ( options.put.pOption ) {
              pOption_t& pOption( options.put.pOption );
              fOptionEngineStop( pOption );
              pOption->StopWatch();
              pOption->OnQuote.Remove( MakeDelegate( this, &Strike::HandleQuote ) );
              //options.put.pOption->OnTrade.Remove( MakeDelegate( this, &Strike::HandleTrade ) );
            }
          }
        }
        --nWatching;
      }
    }

    ~Strike() {
      Stop();
      assert( 0 == nWatching );
    }
  };

  int m_ixFirst, m_ixLast;

  using vRow2Entry_t = std::vector<Strike>;
  vRow2Entry_t m_vRow2Entry;

  virtual int GetNumberRows() override;
  virtual int GetNumberCols() override;

  virtual bool IsEmptyCell( int row, int col ) override;

  virtual wxString GetValue( int row, int col	) override;
  virtual void SetValue( int row, int col, const wxString& ) override;

  virtual void Clear() override;

  virtual bool InsertRows( size_t pos=0, size_t numRows=1 ) override;
  virtual bool AppendRows( size_t numRows=1 ) override;
  virtual bool DeleteRows( size_t pos=0, size_t numRows=1 ) override;

  virtual bool InsertCols( size_t pos=0, size_t numCols=1 ) override;
  virtual bool AppendCols( size_t numCols=1 ) override;
  virtual bool DeleteCols( size_t pos=0, size_t numCols=1 ) override;

  virtual wxGridCellAttr* GetAttr ( int row, int col, wxGridCellAttr::wxAttrKind kind ) override;
  virtual wxString GetColLabelValue( int col ) override;

};