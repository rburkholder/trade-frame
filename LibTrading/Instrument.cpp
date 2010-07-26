/************************************************************************
 * Copyright(c) 2009, One Unified. All rights reserved.                 *
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

#include "StdAfx.h"
#include "Instrument.h"

#include <cassert>

// equity / generic creation
CInstrument::CInstrument(const std::string &sSymbolName, const std::string &sExchangeName,
                         InstrumentType::enumInstrumentTypes type)
: m_sSymbolName( sSymbolName ), m_sExchange( sExchangeName ),
  m_InstrumentType( type ), 
  m_Currency( Currency::USD ), m_CurrencyCounter( Currency::USD ), 
  m_nYear( 0 ), m_nMonth( 0 ), m_OptionSide( OptionSide::Unknown ), m_dblStrike( 0 ),
  m_nContract( 0 )
{
  assert( type < InstrumentType::_Count );
  assert( type > InstrumentType::Unknown );
  assert( 0 < m_sSymbolName.size() );
  //assert( 0 < m_sExchange.size() );
}

 // future
CInstrument::CInstrument( 
    const std::string &sSymbolName, const std::string &sExchangeName,
    InstrumentType::enumInstrumentTypes type, 
    unsigned short year, unsigned short month ) 
    : m_sSymbolName( sSymbolName ), m_sExchange( sExchangeName ),
    m_InstrumentType( type ),
    m_OptionSide( OptionSide::Unknown ), 
    m_Currency( Currency::USD ), m_CurrencyCounter( Currency::USD ), 
    m_nYear( year ), m_nMonth( month ), m_dblStrike( 0 ),
    m_nContract( 0 )
{
  assert( m_InstrumentType == InstrumentType::Future );
  //assert( 0 < m_sSymbolName.size() );
  //assert( 0 < m_sExchange.size() );
}

 // option yymm
CInstrument::CInstrument( 
    const std::string &sSymbolName, const std::string &sExchangeName,
    InstrumentType::enumInstrumentTypes type, 
    unsigned short year, unsigned short month,
    const std::string &sUnderlying,
    OptionSide::enumOptionSide side, 
    double strike ) 
    : m_sSymbolName( sSymbolName ), m_sExchange( sExchangeName ),
    m_sUnderlying( sUnderlying ), m_InstrumentType( type ),
    m_OptionSide( side ), 
    m_Currency( Currency::USD ), m_CurrencyCounter( Currency::USD ), 
    m_nYear( year ), m_nMonth( month ), 
    m_dblStrike( strike ),
    m_nContract( 0 )
{
  assert( side < OptionSide::_Count );
  assert( side > OptionSide::Unknown );
  assert( ( m_InstrumentType == InstrumentType::Option )
       || ( m_InstrumentType == InstrumentType::FuturesOption )
    );
  //assert( 0 < m_sSymbolName.size() );
  //assert( 0 < m_sExchange.size() );
}

 // option yymmdd
CInstrument::CInstrument( 
    const std::string &sSymbolName, const std::string &sExchangeName,
    InstrumentType::enumInstrumentTypes type, 
    unsigned short year, unsigned short month, unsigned short day,
    const std::string &sUnderlying,
    OptionSide::enumOptionSide side, 
    double strike ) 
    : m_sSymbolName( sSymbolName ), m_sExchange( sExchangeName ),
    m_sUnderlying( sUnderlying ), m_InstrumentType( type ),
    m_OptionSide( side ), 
    m_Currency( Currency::USD ), m_CurrencyCounter( Currency::USD ), 
    m_nYear( year ), m_nMonth( month ), m_nDay( day ),
    m_dblStrike( strike ),
    m_nContract( 0 )
{
  assert( side < OptionSide::_Count );
  assert( side > OptionSide::Unknown );
  assert( ( m_InstrumentType == InstrumentType::Option )
       || ( m_InstrumentType == InstrumentType::FuturesOption )
    );
  //assert( 0 < m_sSymbolName.size() );
  //assert( 0 < m_sExchange.size() );
}

// currency
CInstrument::CInstrument(
  const std::string &sSymbolName, const std::string& sUnderlyingName,
  InstrumentType::enumInstrumentTypes type,
  Currency::enumCurrency base, Currency::enumCurrency counter
  ) 
  : m_sSymbolName( sSymbolName ), m_sUnderlying( sUnderlyingName ),
  m_InstrumentType( type ),
  m_Currency( base ), m_CurrencyCounter( counter ),
  m_sExchange( "" ), m_nContract( 0 )
{
  assert( m_InstrumentType == InstrumentType::Currency );
}


CInstrument::CInstrument(const CInstrument& instrument) 
:
  m_sSymbolName( instrument.m_sSymbolName ), 
  m_sUnderlying( instrument.m_sUnderlying ),
  m_sExchange( instrument.m_sExchange ), 
  m_InstrumentType( instrument.m_InstrumentType ), 
  m_Currency( instrument.m_Currency ), 
  m_CurrencyCounter( instrument.m_CurrencyCounter ),
  m_OptionSide( instrument.m_OptionSide ),
  m_nYear( instrument.m_nYear ), 
  m_nMonth( instrument.m_nMonth ), 
  m_nDay( instrument.m_nDay ),
  m_dblStrike( instrument.m_dblStrike ),
  m_nContract( 0 )
{
}

CInstrument::~CInstrument(void) {
}
