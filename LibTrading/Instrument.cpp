#include "StdAfx.h"
#include "assert.h"
#include "Instrument.h"

CInstrument::CInstrument(InstrumentType::enumInstrumentTypes type)
: m_InstrumentType( type )
{
  assert( type < InstrumentType::_Count );
  assert( type >= InstrumentType::Unknown );
}

CInstrument::~CInstrument(void) {
}
