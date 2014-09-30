/************************************************************************
 * Copyright(c) 2014, One Unified. All rights reserved.                 *
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

#include <wx/stattext.h>
#include <wx/sizer.h>
#include <wx/grid.h>

#include <TFVuTrading/ModelCell.h>
#include <TFVuTrading/ModelCell_ops.h>
#include <TFVuTrading/ModelCell_macros.h>

#include "PanelOptionDetails.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

struct PanelOptionDetails_impl {
//public:
  PanelOptionDetails_impl( PanelOptionDetails& );
  ~PanelOptionDetails_impl( void );
//protected:

//private:

  PanelOptionDetails& m_details;

// for column 2, use wxALIGN_LEFT, wxALIGN_CENTRE or wxALIGN_RIGHT
#define GRID_ARRAY_PARAM_COUNT 5
#define GRID_ARRAY_COL_COUNT 13
#define GRID_ARRAY \
  (GRID_ARRAY_COL_COUNT,  \
    ( /* Col 0,         1,            2,         3,      4,             */ \
      (COL_CallBid  , "Bid",   wxALIGN_LEFT,  100, ModelCellString ), \
      (COL_CallLast , "Last",  wxALIGN_RIGHT,  50, ModelCellInt ), \
      (COL_CallAsk  , "Ask",   wxALIGN_LEFT,   50, ModelCellString ), \
      (COL_CallIV   , "IV",    wxALIGN_RIGHT,  50, ModelCellInt ), \
      (COL_CallDelta, "Delta", wxALIGN_LEFT,   50, ModelCellString ), \
      (COL_CallGamma, "Gamma", wxALIGN_RIGHT,  60, ModelCellDouble ), \
      (COL_Strike   , "Desc",  wxALIGN_RIGHT,  60, ModelCellDouble ), \
      (COL_PutBid   , "Bid",   wxALIGN_LEFT,  100, ModelCellString ), \
      (COL_PutLast  , "Last",  wxALIGN_RIGHT,  50, ModelCellInt ), \
      (COL_PutAsk   , "Ask",   wxALIGN_LEFT,   50, ModelCellString ), \
      (COL_PutIV    , "IV",    wxALIGN_RIGHT,  50, ModelCellInt ), \
      (COL_PutDelta , "Delta", wxALIGN_LEFT,   50, ModelCellString ), \
      (COL_PutGamma , "Gamma", wxALIGN_RIGHT,  60, ModelCellDouble ), \
      ) \
    ) \
  /**/

  enum {
    BOOST_PP_REPEAT(GRID_ARRAY_COL_COUNT,GRID_EXTRACT_ENUM_LIST,0)
  };

  typedef boost::fusion::VECTOR_DEF<
    BOOST_PP_REPEAT(GRID_ARRAY_COL_COUNT,COMPOSE_MODEL_CELL,4)
  > vModelCells_t;

  void CreateControls();
  void OnClose( wxCloseEvent& event );

};

} // namespace tf
} // namespace ou