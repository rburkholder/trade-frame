/************************************************************************
 * Copyright(c) 2015, One Unified. All rights reserved.                 *
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
// Started December 30, 2015, 4:51 PM

#pragma once

#define FUSION_MAX_VECTOR_SIZE 13

#include <map>

#include <boost/fusion/container/vector/vector20.hpp>
#include <boost/fusion/include/vector20.hpp>

#include <boost/fusion/algorithm/iteration/for_each.hpp>
#include <boost/fusion/include/for_each.hpp>
#include <boost/fusion/algorithm/iteration/fold.hpp>
#include <boost/fusion/include/fold.hpp>
#include <boost/fusion/sequence/intrinsic/at_c.hpp>
#include <boost/fusion/include/at_c.hpp>
#include <boost/fusion/algorithm/transformation/filter.hpp>
#include <boost/fusion/include/filter.hpp>

//#include <boost/signals2.hpp>

#include <wx/grid.h>

#include <TFVuTrading/ModelCell.h>
#include <TFVuTrading/ModelCell_ops.h>
#include <TFVuTrading/ModelCell_macros.h>

#include "PanelAccountDetails.h"

// modelled after "PanelPortfolioPosition_impl.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

struct PanelAccountDetails_impl {
//public:
  PanelAccountDetails_impl( PanelAccountDetails& );
  virtual ~PanelAccountDetails_impl();
//private:
  
// for column 2, use wxALIGN_LEFT, wxALIGN_CENTRE or wxALIGN_RIGHT
#define GRID_ARRAY_PARAM_COUNT 5
#define GRID_ARRAY_COL_COUNT 12
#define GRID_ARRAY \
  (GRID_ARRAY_COL_COUNT,  \
    ( /* Col 0,            1,            2,         3,      4,             */ \
      (COL_Symbol1  , "Sym1",   wxALIGN_LEFT,   60, ModelCellString ), \
      (COL_Symbol2  , "Sym2",   wxALIGN_LEFT,  120, ModelCellString ), \
      (COL_Expiry   , "Expiry", wxALIGN_LEFT,   75, ModelCellString ), \
      (COL_Exchange , "Exchng", wxALIGN_LEFT,   70, ModelCellString ), \
      (COL_Multiple , "Mltpl",  wxALIGN_RIGHT,  50, ModelCellString ), \
      (COL_Quan     , "Quan",   wxALIGN_RIGHT,  60, ModelCellInt ), \
      (COL_Price    , "Price",  wxALIGN_RIGHT,  60, ModelCellDouble ), \
      (COL_Value    , "Value",  wxALIGN_RIGHT,  80, ModelCellDouble ), \
      (COL_Cost     , "Cost",   wxALIGN_RIGHT,  80, ModelCellDouble ), \
      (COL_UPNL     , "UPNL",   wxALIGN_RIGHT,  80, ModelCellDouble ), \
      (COL_RPNL     , "RPNL",   wxALIGN_RIGHT,  80, ModelCellDouble ), \
      (COL_Currency , "Crncy",  wxALIGN_LEFT,   50, ModelCellString ), \
      ) \
    ) \
  /**/

  enum {
    BOOST_PP_REPEAT(GRID_ARRAY_COL_COUNT,GRID_EXTRACT_ENUM_LIST,0)
  };

  typedef boost::fusion::VECTOR_DEF<
    BOOST_PP_REPEAT(GRID_ARRAY_COL_COUNT,COMPOSE_MODEL_CELL,4)
  > vModelCells_t;

  class AccountDetailRow {
  public:
    AccountDetailRow( wxGrid* pGrid, int row ): m_pGrid( pGrid ), m_row( row ) { Init(); }
    AccountDetailRow( const AccountDetailRow& rhs ): m_pGrid( rhs.m_pGrid ), m_row( rhs.m_row ) { Init(); }
    ~AccountDetailRow( void ) {}
    void UpdateGui( void ) {
      boost::fusion::for_each( m_vModelCells, ModelCell_ops::UpdateGui( m_pGrid, m_row ) );
    }
    void UpdateAccountDetail( const ou::tf::IBTWS::AccountDetails& ad ) {
      boost::fusion::at_c<COL_Symbol1>( m_vModelCells ).SetValue( ad.sSymbol );
      boost::fusion::at_c<COL_Symbol2>( m_vModelCells ).SetValue( ad.sLocalSymbol );
      boost::fusion::at_c<COL_Expiry>( m_vModelCells ).SetValue( ad.sExpiry );
      boost::fusion::at_c<COL_Exchange>( m_vModelCells ).SetValue( ad.sExchange );
      boost::fusion::at_c<COL_Multiple>( m_vModelCells ).SetValue( ad.sMultiplier );
      boost::fusion::at_c<COL_Quan>( m_vModelCells ).SetValue( ad.position );
      boost::fusion::at_c<COL_Price>( m_vModelCells ).SetValue( ad.marketPrice );
      boost::fusion::at_c<COL_Value>( m_vModelCells ).SetValue( ad.marketValue );
      boost::fusion::at_c<COL_Cost>( m_vModelCells ).SetValue( ad.averageCost );
      boost::fusion::at_c<COL_UPNL>( m_vModelCells ).SetValue( ad.unrealizedPNL );
      boost::fusion::at_c<COL_RPNL>( m_vModelCells ).SetValue( ad.realizedPNL );
      boost::fusion::at_c<COL_Currency>( m_vModelCells ).SetValue( ad.sCurrency );
    }
  protected:
  private:
    wxGrid* m_pGrid;
    int m_row;
    vModelCells_t m_vModelCells;
    
    void Init( void ) {
      boost::fusion::fold( m_vModelCells, 0, ModelCell_ops::SetCol() );
      BOOST_PP_REPEAT(GRID_ARRAY_COL_COUNT,COL_ALIGNMENT,m_row)
      
    }
  };
  
  wxGrid* m_pGrid;  // for use in macro GRID_EMIT_SetColSettings
  
  PanelAccountDetails& m_pad; // passed in on construction 
  
  typedef std::map<std::string,AccountDetailRow> mapAccountDetailRow_t;
  mapAccountDetailRow_t m_mapAccountDetailRow;

  void CreateControls();
  
  void UpdateAccountDetailRow( const ou::tf::IBTWS::AccountDetails& ad );
  
  void OnClose( wxCloseEvent& event );
};

} // namespace tf
} // namespace ou


