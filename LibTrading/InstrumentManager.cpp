#include "StdAfx.h"
#include "InstrumentManager.h"

#include <stdexcept>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CInstrumentManager::CInstrumentManager(void) {
  file.OpenIQFSymbols();
}

CInstrumentManager::~CInstrumentManager(void) {
  file.CloseIQFSymbols();
}

CInstrument::pInstrument_t CInstrumentManager::GetIQFeedInstrument(const std::string &sName) {
  return GetIQFeedInstrument( sName, sName );
}

CInstrument::pInstrument_t CInstrumentManager::GetIQFeedInstrument(const std::string &sName, const std::string &sAlternateName) {
  try {
    CInstrument::pInstrument_t pInstrument( file.CreateInstrumentFromIQFeed( sName, sAlternateName ) );  // todo:  need to verify proper symbol usage
    return pInstrument;
  }
  catch (...) {
    throw std::runtime_error( "CBasketTradeSymbolInfo::Initialize problems" );
  }
  //return pInstrument;
}




