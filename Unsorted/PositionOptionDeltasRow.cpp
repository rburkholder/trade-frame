#include "StdAfx.h"

#include <stdexcept>

#include "PositionOptionDeltasRow.h"

CPositionOptionDeltasRow::CPositionOptionDeltasRow(
  vDeltaRows_t::size_type ixRow,
  CProviderInterface *pDataProvider, CInstrument::pInstrument_t pInstrument
  ) 
  : m_ixRow( ixRow ), 
    m_pDataProvider( pDataProvider ), m_pInstrument( pInstrument ) 
{
  m_viSymbol = pInstrument->GetSymbolName();

  std::stringstream ss;
  ss << pInstrument->GetUnderlyingName() 
    << " " << OptionSide::ShortName[ pInstrument->GetOptionSide() ]
    << " " << pInstrument->GetExpiryYear()
    << "/" << pInstrument->GetExpiryMonth()
    << " " << pInstrument->GetStrike();
  m_viSymbolDesc = ss.str();

  // preallocate the vector positions
  m_vVisibleItems.reserve( COLHDR_DELTAS_ARRAY_ROW_COUNT );
  // assign the appropriate blank value to each item
  BOOST_PP_REPEAT( BOOST_PP_ARRAY_SIZE( COLHDR_DELTAS_ARRAY ), COLHDR_DELTAS_EMIT_SetBlank, .SetBlank )
  // assign to each vector element, a pointer to each of the visible items
  BOOST_PP_REPEAT( BOOST_PP_ARRAY_SIZE( COLHDR_DELTAS_ARRAY ), COLHDR_DELTAS_EMIT_AssignToVector, ~ )

  m_pDataProvider->AddQuoteHandler( m_viSymbol.String(), MakeDelegate( this, &CPositionOptionDeltasRow::HandleQuote ) );
  m_pDataProvider->AddTradeHandler( m_viSymbol.String(), MakeDelegate( this, &CPositionOptionDeltasRow::HandleTrade ) ) ;
}

CPositionOptionDeltasRow::~CPositionOptionDeltasRow(void) {
  m_pDataProvider->RemoveQuoteHandler( m_viSymbol.String(), MakeDelegate( this, &CPositionOptionDeltasRow::HandleQuote ) );
  m_pDataProvider->RemoveTradeHandler( m_viSymbol.String(), MakeDelegate( this, &CPositionOptionDeltasRow::HandleTrade ) ) ;
}

void CPositionOptionDeltasRow::HandleQuote( CSymbol::quote_t quote ) {

  m_viBid = quote.m_dblBid;
  m_viAsk = quote.m_dblAsk;
  m_viAskSize = quote.m_nAskSize;
  m_viBidSize = quote.m_nBidSize;
  m_viSpread = quote.m_dblAsk >= quote.m_dblBid ? quote.m_dblAsk - quote.m_dblBid : quote.m_dblBid - quote.m_dblAsk;
  
  if ( NULL != OnRowUpdated ) OnRowUpdated( m_ixRow );
}

void CPositionOptionDeltasRow::HandleTrade( CSymbol::trade_t trade ) {
  // update the variables then:
  if ( NULL != OnRowUpdated ) OnRowUpdated( m_ixRow );
}