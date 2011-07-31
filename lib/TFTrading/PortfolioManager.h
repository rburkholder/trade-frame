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
#include <set>
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
  typedef std::set<idPortfolio_t> setPortfolioId_t;

  typedef CPosition::pPosition_t pPosition_t;
  typedef keytypes::idPosition_t idPosition_t;
  typedef keytypes::idAccount_t idAccount_t;
  typedef keytypes::idInstrument_t idInstrument_t;

  typedef keytypes::idAccountOwner_t idAccountOwner_t;

  typedef CInstrument::pInstrument_cref pInstrument_cref;
  typedef CPosition::pProvider_t pProvider_t;

  typedef std::pair<const CPosition&, const CExecution&> execution_pair_t;
  typedef const execution_pair_t& execution_delegate_t;

  typedef std::pair<std::string, pPosition_t> mapPosition_pair_t;
  typedef std::map<std::string, pPosition_t> mapPosition_t;
  typedef mapPosition_t::iterator iterPosition_t;

  struct structPortfolio {
    pPortfolio_t pPortfolio;
    mapPosition_t mapPosition;
    structPortfolio( pPortfolio_t& pPortfolio_ ) : pPortfolio( pPortfolio_ ) {};
  };

  typedef std::pair<idPortfolio_t, structPortfolio> mapPortfolio_pair_t;
  typedef std::map<idPortfolio_t, structPortfolio> mapPortfolios_t;
  typedef mapPortfolios_t::iterator iterPortfolio_t;

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

  typedef FastDelegate1<pPosition_t&> OnPositionNeedsDetailsHandler;
  void SetOnPositionNeedDetails( OnPositionNeedsDetailsHandler function ) {
    OnPositionNeedsDetails = function;
  }

  void LoadActivePortfolios( void );

  void AttachToSession( ou::db::CSession* pSession );
  void DetachFromSession( ou::db::CSession* pSession );

protected:

private:

  mapPortfolios_t m_mapPortfolios;

  typedef std::map<idPortfolio_t,setPortfolioId_t> mapReportingPortfolios_t;
  typedef std::pair<idPortfolio_t,setPortfolioId_t> mapReportingPortfolios_pair_t;
  mapReportingPortfolios_t m_mapReportingPortfolios;
  typedef mapReportingPortfolios_t::iterator iterReportingPortfolios_t;

  void UpdateReportingPortfolio( idPortfolio_t idOwner, idPortfolio_t idReporting );

  OnPositionNeedsDetailsHandler OnPositionNeedsDetails;

  void HandleRegisterTables( ou::db::CSession& session );
  void HandleRegisterRows( ou::db::CSession& session );
  void HandlePopulateTables( ou::db::CSession& session );

  void HandlePositionOnExecution( execution_delegate_t );
  void HandlePositionOnCommission( const CPosition* );

  void HandlePortfolioOnExecution( const CPortfolio* );
  void HandlePortfolioOnCommission( const CPortfolio* );

  void LoadPositions( const idPortfolio_t& idPortfolio, mapPosition_t& mapPosition );

};


} // namespace tf
} // namespace ou
