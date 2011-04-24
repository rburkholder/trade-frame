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

#include <OUCommon/FastDelegate.h>
using namespace fastdelegate;

#include "KeyTypes.h"

#include "ManagerBase.h"
#include "Portfolio.h"
#include "Position.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

class CPortfolioManager: public ManagerBase<CPortfolioManager> {
public:

  typedef CPortfolio::pPortfolio_t pPortfolio_t;
  typedef keytypes::idPortfolio_t idPortfolio_t;

  typedef CPosition::pPosition_t pPosition_t;
  typedef keytypes::idPosition_t idPosition_t;
  typedef keytypes::idAccount_t idAccount_t;
  typedef keytypes::idInstrument_t idInstrument_t;

  typedef keytypes::idAccountOwner_t idAccountOwner_t;

  typedef CInstrument::pInstrument_cref pInstrument_cref;
  typedef CPosition::pProvider_t pProvider_t;

  typedef std::pair<const CPosition&, const CExecution&> execution_pair_t;
  typedef const execution_pair_t& execution_delegate_t;

  CPortfolioManager(void) {};
  ~CPortfolioManager(void) {};

  pPortfolio_t ConstructPortfolio( 
    const idPortfolio_t& idPortfolio, const idAccountOwner_t& idAccountOwner, const std::string& sDescription = "" );
  pPortfolio_t GetPortfolio( const idPortfolio_t& idPortfolio );
  void UpdatePortfolio( const idPortfolio_t& idPortfolio );
  void DeletePortfolio( const idPortfolio_t& idPortfolio );

  pPosition_t ConstructPosition( 
    const idPortfolio_t& idPortfolio, const std::string& sName, const std::string& sAlgorithm,
    const idAccount_t& idExecutionAccount, const idAccount_t& idDataAccount, 
    const pProvider_t& pExecutionProvider, const pProvider_t& pDataProvider,
    pInstrument_cref pInstrument
    );
  pPosition_t GetPosition( const idPortfolio_t& idPortfolio, const std::string& sName );
  void UpdatePosition( const idPortfolio_t& idPortfolio, const std::string& sName );
  void DeletePosition( const idPortfolio_t& idPortfolio, const std::string& sName );

  void RegisterTablesForCreation( void );
  void RegisterRowDefinitions( void );
  void PopulateTables( void );

  typedef FastDelegate1<pPosition_t&> OnPositionNeedsDetailsHandler;
  void SetOnPositionNeedDetails( OnPositionNeedsDetailsHandler function ) {
    OnPositionNeedsDetails = function;
  }

protected:

private:

  typedef std::pair<std::string, pPosition_t> mapPosition_pair_t;
  typedef std::map<std::string, pPosition_t> mapPosition_t;
  typedef mapPosition_t::iterator iterPosition_t;

  struct structPortfolio {
    pPortfolio_t pPortfolio;
    mapPosition_t mapPosition;
    structPortfolio( pPortfolio_t& pPortfolio_ ) : pPortfolio( pPortfolio_ ) {};
  };

  typedef std::pair<idPortfolio_t, structPortfolio> mapPortfolio_pair_t;
  typedef std::map<idPortfolio_t, structPortfolio> mapPortfolio_t;
  typedef mapPortfolio_t::iterator iterPortfolio_t;
  mapPortfolio_t m_mapPortfolio;

  OnPositionNeedsDetailsHandler OnPositionNeedsDetails;

  void HandlePositionOnExecution( execution_delegate_t );
  void HandlePositionOnCommission( const CPosition* );

  void HandlePortfolioOnExecution( const CPortfolio* );
  void HandlePortfolioOnCommission( const CPortfolio* );

};


} // namespace tf
} // namespace ou
