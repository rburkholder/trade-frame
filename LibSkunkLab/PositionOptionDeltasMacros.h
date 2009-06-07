#pragma once

#include "boost/preprocessor/tuple/elem.hpp"
#include "boost/preprocessor/array/elem.hpp"
#include "boost/preprocessor/array/size.hpp"
#include "boost/preprocessor/punctuation/comma_if.hpp"
#include "boost/preprocessor/repetition/repeat.hpp"

#define COLHDR_DELTAS_ARRAY_ELEMENT_SIZE 6
#define COLHDR_DELTAS_ARRAY \
  (13, /* number of rows */ \
    ( /* Col 0, 1, 2, 3, 4, 5 */ \
      (COLHDR_DELTAS_COL_Sym   , "Sym",    LVCFMT_RIGHT, 50, std::string, m_sSymbol), \
      (COLHDR_DELTAS_COL_Desc  , "Desc",   LVCFMT_LEFT,  50, std::string, m_sSymbolDesc), \
      (COLHDR_DELTAS_COL_Bid   , "Bid",    LVCFMT_RIGHT, 50, double,      m_dblBid), \
      (COLHDR_DELTAS_COL_BidSz , "BidSz",  LVCFMT_RIGHT, 50, int,         m_nBidSize), \
      (COLHDR_DELTAS_COL_Sprd  , "Sprd",   LVCFMT_RIGHT, 50, double,      m_dblSpread), \
      (COLHDR_DELTAS_COL_Ask   , "Ask",    LVCFMT_RIGHT, 50, double,      m_dblAsk), \
      (COLHDR_DELTAS_COL_AskSz , "AskSz",  LVCFMT_RIGHT, 50, int,         m_nAskSize), \
      (COLHDR_DELTAS_COL_Pos   , "Pos",    LVCFMT_RIGHT, 50, int,         m_nPosition), \
      (COLHDR_DELTAS_COL_AvgCst, "AvgCst", LVCFMT_RIGHT, 50, double,      m_dblAverageCost), \
      (COLHDR_DELTAS_COL_Delta , "Delta",  LVCFMT_RIGHT, 50, double,      m_dblDelta), \
      (COLHDR_DELTAS_COL_Gamma , "Gamma",  LVCFMT_RIGHT, 50, double,      m_dblGamma), \
      (COLHDR_DELTAS_COL_UnRlPL, "UnRlPL", LVCFMT_RIGHT, 50, double,      m_dblUnrealizedPL), \
      (COLHDR_DELTAS_COL_RlPL  , "RlPL",   LVCFMT_RIGHT, 50, double,      m_dblRealizedPL) \
      ) \
    ) \
  /**/

#define COLHDR_DELTAS_EXTRACT_COL_DETAILS(z, n, m, text) \
  BOOST_PP_TUPLE_ELEM( \
    COLHDR_DELTAS_ARRAY_ELEMENT_SIZE, m, \
      BOOST_PP_ARRAY_ELEM( n, COLHDR_DELTAS_ARRAY ) \
    )

#define COLHDR_DELTAS_EXTRACT_ENUM_LIST(z, n, text) \
  BOOST_PP_COMMA_IF(n) \
  COLHDR_DELTAS_EXTRACT_COL_DETAILS( z, n, 0, text )

#define COLHDR_DELTAS_EMIT_InsertColumn( z, n, VAR ) \
  CListCtrl::InsertColumn( VAR++, \
    _T(COLHDR_DELTAS_EXTRACT_COL_DETAILS(z, n, 1, ~)), \
    COLHDR_DELTAS_EXTRACT_COL_DETAILS(z, n, 2, ~), \
    COLHDR_DELTAS_EXTRACT_COL_DETAILS(z, n, 3, ~) \
    );

#define COLHDR_DELTAS_EMIT_DefineVars( z, n, text ) \
  COLHDR_DELTAS_EXTRACT_COL_DETAILS(z, n, 4, ~) \
  COLHDR_DELTAS_EXTRACT_COL_DETAILS(z, n, 5, ~)\
  ;

