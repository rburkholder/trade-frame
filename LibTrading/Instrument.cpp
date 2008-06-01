#include "StdAfx.h"
#include "assert.h"
#include "Instrument.h"

CInstrument::CInstrument(const std::string &sSymbolName, 
                         InstrumentType::enumInstrumentTypes type)
: m_sSymbolName( sSymbolName ), m_InstrumentType( type ), 
  m_Currency( Currency::USD ), m_Exchange( Exchange::SMART ),
  m_nYear( 0 ), m_nMonth( 0 ), m_OptionSide( OptionSide::Unknown ), m_dblStrike( 0 )
{
  assert( type < InstrumentType::_Count );
  assert( type > InstrumentType::Unknown );
}

 // future
CInstrument::CInstrument( 
    const std::string &sSymbol, 
    InstrumentType::enumInstrumentTypes type, 
    unsigned short year, unsigned short month ) 
    : m_sSymbolName( sSymbol ), m_InstrumentType( type ),
    m_OptionSide( OptionSide::Unknown ), m_Currency( Currency::USD ), 
    m_Exchange( Exchange::SMART ), m_nYear( year ), m_nMonth( month ), m_dblStrike( 0 )
{
}

 // option
CInstrument::CInstrument( 
    const std::string &sSymbolName, 
    InstrumentType::enumInstrumentTypes type, 
    unsigned short year, unsigned short month,
    const std::string &sUnderlying,
    OptionSide::enumOptionSide side, 
    double strike ) 
    : m_sSymbolName( sSymbolName ), m_sUnderlying( sUnderlying ), m_InstrumentType( type ),
    m_OptionSide( side ), m_Currency( Currency::USD ), 
    m_Exchange( Exchange::SMART ), 
    m_nYear( year ), m_nMonth( month ), m_dblStrike( strike )
{
}

CInstrument::CInstrument(const CInstrument &instrument) 
:
  m_sSymbolName( instrument.m_sSymbolName ), 
  m_sUnderlying( instrument.m_sUnderlying ), 
  m_InstrumentType( instrument.m_InstrumentType ), 
  m_Currency(  instrument.m_Currency ), 
  m_Exchange( instrument.m_Exchange ), 
  m_nYear( instrument.m_nYear ), 
  m_nMonth( instrument.m_nMonth ), 
  m_dblStrike( instrument.m_dblStrike )
{
}

CInstrument::~CInstrument(void) {
}
