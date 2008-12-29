#include "StdAfx.h"
#include "assert.h"
#include "Instrument.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CInstrument::CInstrument(const std::string &sSymbolName, const std::string &sExchangeName,
                         InstrumentType::enumInstrumentTypes type)
: m_sSymbolName( sSymbolName ), m_sExchange( sExchangeName ),
  m_InstrumentType( type ), 
  m_Currency( Currency::USD ),
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
    m_OptionSide( OptionSide::Unknown ), m_Currency( Currency::USD ), 
    m_nYear( year ), m_nMonth( month ), m_dblStrike( 0 ),
    m_nContract( 0 )
{
  //assert( 0 < m_sSymbolName.size() );
  //assert( 0 < m_sExchange.size() );
}

 // option
CInstrument::CInstrument( 
    const std::string &sSymbolName, const std::string &sExchangeName,
    InstrumentType::enumInstrumentTypes type, 
    unsigned short year, unsigned short month,
    const std::string &sUnderlying,
    OptionSide::enumOptionSide side, 
    double strike ) 
    : m_sSymbolName( sSymbolName ), m_sExchange( sExchangeName ),
    m_sUnderlying( sUnderlying ), m_InstrumentType( type ),
    m_OptionSide( side ), m_Currency( Currency::USD ), 
    m_nYear( year ), m_nMonth( month ), m_dblStrike( strike ),
    m_nContract( 0 )
{
  assert( side < OptionSide::_Count );
  assert( side > OptionSide::Unknown );
  //assert( 0 < m_sSymbolName.size() );
  //assert( 0 < m_sExchange.size() );
}

CInstrument::CInstrument(const CInstrument &instrument) 
:
  m_sSymbolName( instrument.m_sSymbolName ), 
  m_sExchange( instrument.m_sExchange ), 
  m_sUnderlying( instrument.m_sUnderlying ), 
  m_InstrumentType( instrument.m_InstrumentType ), 
  m_Currency(  instrument.m_Currency ), 
  m_nYear( instrument.m_nYear ), 
  m_nMonth( instrument.m_nMonth ), 
  m_dblStrike( instrument.m_dblStrike ),
  m_nContract( 0 )
{
}

CInstrument::~CInstrument(void) {
}
