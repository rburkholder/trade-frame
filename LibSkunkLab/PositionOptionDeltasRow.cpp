#include "StdAfx.h"

#include <stdexcept>

#include "PositionOptionDeltasRow.h"

CPositionOptionDeltasRow::CPositionOptionDeltasRow(
  vDeltaRows_t::size_type ixRow,
  CProviderInterface *pDataProvider, CInstrument::pInstrument_t pInstrument
  ) 
  : m_ixRow( ixRow ), 
    m_pDataProvider( pDataProvider ), m_pInstrument( pInstrument ), m_sSymbol( pInstrument->GetSymbolName() )
{
  std::stringstream ss;
  ss << pInstrument->GetUnderlyingName() 
    << " " << OptionSide::ShortName[ pInstrument->GetOptionSide() ]
    << " " << pInstrument->GetExpiryYear()
    << "/" << pInstrument->GetExpiryMonth()
    << " " << pInstrument->GetStrike();
  m_sSymbolDesc = ss.str().c_str();

  m_pDataProvider->AddQuoteHandler( m_sSymbol, MakeDelegate( this, &CPositionOptionDeltasRow::HandleQuote ) );
  m_pDataProvider->AddTradeHandler( m_sSymbol, MakeDelegate( this, &CPositionOptionDeltasRow::HandleTrade ) ) ;
}

CPositionOptionDeltasRow::~CPositionOptionDeltasRow(void) {
  m_pDataProvider->RemoveQuoteHandler( m_sSymbol, MakeDelegate( this, &CPositionOptionDeltasRow::HandleQuote ) );
  m_pDataProvider->RemoveTradeHandler( m_sSymbol, MakeDelegate( this, &CPositionOptionDeltasRow::HandleTrade ) ) ;
}

void CPositionOptionDeltasRow::HandleQuote( CSymbol::quote_t quote ) {
}

void CPositionOptionDeltasRow::HandleTrade( CSymbol::trade_t trade ) {
}