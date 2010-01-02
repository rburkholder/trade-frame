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

#pragma once

#include <map>
#include <string>
#include <stdexcept>

#include "boost/shared_ptr.hpp"

#include "LibCommon/Delegate.h"

#include "Symbol.h"
#include "Order.h"
#include "AlternateInstrumentNames.h"
#include "OrderManager.h"
#include "Portfolio.h"

// need to include a check that callbacks and virtuals are in the correct thread
// in IB, processMsg may be best place to have in cross thread management, if it isn't already

// Merge CInstrument and CSymbol?
// Create CSymbol from CInstrument?
// Store CInstrument in CSymbol?  <= use this with smart_ptr on CInstrument.

class CProviderInterface {
public:
  CProviderInterface(void);
  virtual ~CProviderInterface(void);

  typedef boost::shared_ptr<CProviderInterface> pProvider_t;

  virtual void Connect( void );
  Delegate<int> OnConnected;
  virtual void Disconnect( void );
  Delegate<int> OnDisconnected;

  virtual void AddOnOpenHandler( const string &sSymbol, CSymbol::tradehandler_t handler );
  virtual void RemoveOnOpenHandler( const string &sSymbol, CSymbol::tradehandler_t handler );
  virtual void AddQuoteHandler( const string &sSymbol, CSymbol::quotehandler_t handler );
  virtual void RemoveQuoteHandler( const string &sSymbol, CSymbol::quotehandler_t handler );
  virtual void AddTradeHandler( const string &sSymbol, CSymbol::tradehandler_t handler );
  virtual void RemoveTradeHandler( const string &sSymbol, CSymbol::tradehandler_t handler );
  virtual void AddDepthHandler( const string &sSymbol, CSymbol::depthhandler_t handler );
  virtual void RemoveDepthHandler( const string &sSymbol, CSymbol::depthhandler_t handler );

  Delegate<CPortfolio::UpdatePortfolioRecord_t> OnUpdatePortfolioRecord;  // need to do the Add/Remove thing

  virtual CSymbol* GetSymbol( const string &sSymbol );

  const std::string &Name( void ) { return m_sName; };
  unsigned short ID( void ) { assert( 0 != m_nID ); return m_nID; };
  bool Connected( void ) { return m_bConnected; };

  virtual void PlaceOrder( COrder *pOrder );
  virtual void CancelOrder( COrder *pOrder );

  void SetAlternateInstrumentName( const std::string &OriginalInstrumentName, const std::string &AlternateIntrumentName );
  void GetAlternateInstrumentName( const std::string &OriginalInstrumentName, std::string *pAlternateInstrumentName );

  enum enumProviderId { EProviderIB = 100, EProviderIQF, EProviderGNDT, EProviderSimulator, _EProviderCount };
protected:
  std::string m_sName;  // name of provider
  unsigned short m_nID;
  bool m_bConnected;

  typedef std::map<const std::string, CSymbol*> m_mapSymbols_t;
  typedef std::pair<const std::string, CSymbol*> pair_mapSymbols_t;
  m_mapSymbols_t m_mapSymbols;

  virtual void StartQuoteWatch( CSymbol *pSymbol ) {};
  virtual void StopQuoteWatch( CSymbol *pSymbol ) {};
  virtual void StartTradeWatch( CSymbol *pSymbol ) {};
  virtual void StopTradeWatch( CSymbol *pSymbol ) {};
  virtual void StartDepthWatch( CSymbol *pSymbol ) {};
  virtual void StopDepthWatch( CSymbol *pSymbol ) {};

  virtual CSymbol *NewCSymbol( const std::string &sSymbolName ) = 0; // override needs to call AddCSymbol to add symbol to map
  CSymbol* AddCSymbol( CSymbol* pSymbol );

  std::map<std::string, std::string> m_mapAlternateNames;  // caching map to save database lookups
  CAlternateInstrumentNames m_lutAlternateInstrumentNames;

  //COrderManager m_OrderManager;

  //virtual void PreSymbolDestroy( CSymbol *pSymbol );

private:
};

/*

Discussion of calling sequence for open, quote, trade, depth handlers:
* client application calls Provider to add a handler
* CProviderInterface maintains list of symbols, 
   and will use the pure virtual override to create a new one when necessary 

*/


// 
