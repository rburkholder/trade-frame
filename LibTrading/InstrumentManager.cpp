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

CInstrument* CInstrumentManager::GetIQFeedInstrument(const std::string &sName) {
  return GetIQFeedInstrument( sName, sName );
}

CInstrument* CInstrumentManager::GetIQFeedInstrument(const std::string &sName, const std::string &sAlternateName) {
  CInstrument* pInstrument = NULL;
  try {
    pInstrument = file.CreateInstrumentFromIQFeed( sName, sAlternateName );  // todo:  need to verify proper symbol usage
  }
  catch (...) {
    std::cout << "CBasketTradeSymbolInfo::Initialize problems" << std::endl;
  }
  return pInstrument;
}




