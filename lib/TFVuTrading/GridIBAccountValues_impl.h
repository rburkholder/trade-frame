/************************************************************************
 * Copyright(c) 2016, One Unified. All rights reserved.                 *
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
// Started January 3, 2016, 3:31 PM

#pragma once

#include <map>

#include <boost/fusion/container/vector/vector10.hpp>
#include <boost/fusion/include/vector10.hpp>

#include <boost/fusion/algorithm/iteration/for_each.hpp>
#include <boost/fusion/include/for_each.hpp>
#include <boost/fusion/algorithm/iteration/fold.hpp>
#include <boost/fusion/include/fold.hpp>
#include <boost/fusion/sequence/intrinsic/at_c.hpp>
#include <boost/fusion/include/at_c.hpp>
#include <boost/fusion/algorithm/transformation/filter.hpp>
#include <boost/fusion/include/filter.hpp>

#include <boost/serialization/version.hpp>
#include <boost/serialization/split_member.hpp>

#include <wx/grid.h>

#include <TFVuTrading/ModelCell.h>
#include <TFVuTrading/ModelCell_ops.h>
#include <TFVuTrading/ModelCell_macros.h>

#include "GridIBAccountValues.h"

// modeled after "PanelPortfolioPosition_impl.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

struct PanelIBAccountValues_impl {
//public:
  PanelIBAccountValues_impl( PanelIBAccountValues& );
  virtual ~PanelIBAccountValues_impl();
//private:
  
// for columns: wxALIGN_LEFT, wxALIGN_CENTRE or wxALIGN_RIGHT
#define GRID_ARRAY_PARAM_COUNT 5
#define GRID_ARRAY_COL_COUNT 4
#define GRID_ARRAY \
  (GRID_ARRAY_COL_COUNT,  \
    ( /* Col 0,            1,            2,         3,      4,             */ \
      (COL_Key      , "Key",     wxALIGN_LEFT,   120, ModelCellString ), \
      (COL_Value    , "Value",   wxALIGN_RIGHT,  100, ModelCellString ), \
      (COL_Currency , "Crncy",   wxALIGN_LEFT,    75, ModelCellString ), \
      (COL_Account  , "Account", wxALIGN_LEFT,    80, ModelCellString ), \
      ) \
    ) \
  /**/

  enum {
    BOOST_PP_REPEAT(GRID_ARRAY_COL_COUNT,GRID_EXTRACT_ENUM_LIST,0)
  };

  typedef boost::fusion::VECTOR_DEF<
    BOOST_PP_REPEAT(GRID_ARRAY_COL_COUNT,COMPOSE_MODEL_CELL,4)
  > vModelCells_t;

  class AccountValueRow {
  public:
    AccountValueRow( wxGrid& grid, int nRow ): m_grid( grid ), m_nRow( nRow ) { Init(); }
    AccountValueRow( const AccountValueRow& rhs ): m_grid( rhs.m_grid ), m_nRow( rhs.m_nRow ) { Init(); }
    ~AccountValueRow( void ) {}
    void UpdateGui( void ) {
      boost::fusion::for_each( m_vModelCells, ModelCell_ops::UpdateGui( m_grid, m_nRow ) );
    }
    void UpdateAccountValue( const ou::tf::IBTWS::AccountValue& av ) {
      boost::fusion::at_c<COL_Key>( m_vModelCells ).SetValue( av.sKey );
      boost::fusion::at_c<COL_Value>( m_vModelCells ).SetValue( av.sVal );
      boost::fusion::at_c<COL_Currency>( m_vModelCells ).SetValue( av.sCurrency );
      boost::fusion::at_c<COL_Account>( m_vModelCells ).SetValue( av.sAccountName );
    }
  protected:
  private:
    wxGrid& m_grid;
    int m_nRow;
    vModelCells_t m_vModelCells;
    
    void Init( void ) {
      boost::fusion::fold( m_vModelCells, 0, ModelCell_ops::SetCol() );
      BOOST_PP_REPEAT(GRID_ARRAY_COL_COUNT,COL_ALIGNMENT,m_nRow)
    }
  };
  
  // need to change the whole thing from panel to grid, emulate what was done with GridOptionDetails
  wxGrid* m_pGrid;  // for use in macro GRID_EMIT_SetColSettings
  
  PanelIBAccountValues& m_pav; // passed in on construction 
  
  typedef std::map<std::string,AccountValueRow> mapAccountValueRow_t;
  mapAccountValueRow_t m_mapAccountValueRow;

  void CreateControls();
  
  void UpdateAccountValueRow( const ou::tf::IBTWS::AccountValue& av );
  
  void OnClose( wxCloseEvent& event );
  
  template<typename Archive>
  void save( Archive& ar, const unsigned int version ) const {
    int cnt = m_pGrid->GetCols();
    ar & cnt;
    for ( int ix = 0; ix < cnt; ix++ ) {
      ar & m_pGrid->GetColumnWidth( ix );
    }
  }

  template<typename Archive>
  void load( Archive& ar, const unsigned int version ) {
    int cnt;
    ar & cnt;
    assert( cnt == m_pGrid->GetCols() ); 
    int width;
    for ( int ix = 0; ix < cnt; ix++ ) {
      ar & width;
      m_pGrid->SetColumnWidth( ix, width );
    }
  }

  BOOST_SERIALIZATION_SPLIT_MEMBER()
};

template<class Archive>
void PanelIBAccountValues::serialize(Archive & ar, const unsigned int file_version){
    ar & *m_pimpl;
}  

} // namespace tf
} // namespace ou

BOOST_CLASS_VERSION(ou::tf::PanelIBAccountValues_impl, 1)
