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

#include <boost/range.hpp>
#include <boost/range/algorithm/for_each.hpp>
#include <boost/range/adaptor/map.hpp>
//#include <boost/range/adaptor/

#include <OUCommon/FastDelegate.h>
using namespace fastdelegate;
#include <OUCommon/Delegate.h>
#include <OUCommon/ManagerBase.h>

#include "KeyTypes.h"

#include "Portfolio.h"
#include "Position.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

class PortfolioManager: public ou::db::ManagerBase<PortfolioManager> {
public:

  using pPortfolio_t = Portfolio::pPortfolio_t;
  using idPortfolio_t = keytypes::idPortfolio_t;
  using setPortfolioId_t = std::set<idPortfolio_t>;

  using EPortfolioType = Portfolio::EPortfolioType;
  using currency_t = Portfolio::currency_t;

  using pPosition_t = Position::pPosition_t;
  using idPosition_t = keytypes::idPosition_t;
  using idAccount_t = keytypes::idAccount_t;
  using idInstrument_t = keytypes::idInstrument_t;

  using idAccountOwner_t = keytypes::idAccountOwner_t;

  using pWatch_t = Position::pWatch_t;
  using pInstrument_t = Instrument::pInstrument_t;
  using pProvider_t = Position::pProvider_t;

  using execution_pair_t = std::pair<const Position&, const Execution&>;
  using execution_delegate_t = const execution_pair_t&;

  using mapPosition_pair_t = std::pair<std::string, pPosition_t>;
  using mapPosition_t = std::map<std::string, pPosition_t>;
  using mapPosition_iter_t = mapPosition_t::iterator;

  struct structPortfolio {
    pPortfolio_t pPortfolio;
    mapPosition_t mapPosition;
    structPortfolio( pPortfolio_t& pPortfolio_ ) : pPortfolio( pPortfolio_ ) {};
  };

  PortfolioManager() {};
  ~PortfolioManager() {};

  bool PortfolioExists( const idPortfolio_t& idPortfolio );
  bool PositionExists( const idPortfolio_t& idPortfolio, const std::string& sName );

  pPortfolio_t ConstructPortfolio(
    const idPortfolio_t& idPortfolio, const idAccountOwner_t& idAccountOwner, const idPortfolio_t& idOwner,
    EPortfolioType ePortfolioType, currency_t eCurrency, const std::string& sDescription = "" );
  pPortfolio_t GetPortfolio( const idPortfolio_t& idPortfolio );
  void UpdatePortfolio( const idPortfolio_t& idPortfolio );
  void DeletePortfolio( const idPortfolio_t& idPortfolio );

  pPosition_t ConstructPosition( // old mechanism
    const idPortfolio_t& idPortfolio, const std::string& sName, const std::string& sAlgorithm,
    const idAccount_t& idExecutionAccount, const idAccount_t& idDataAccount,
    const pProvider_t& pExecutionProvider, const pProvider_t& pDataProvider,
    pInstrument_t pInstrument
    );

  pPosition_t ConstructPosition( // new mechanism
    const idPortfolio_t& idPortfolio, const std::string& sName, const std::string& sAlgorithm,
    const idAccount_t& idExecutionAccount, const idAccount_t& idDataAccount,
    const pProvider_t& pExecutionProvider,
    pWatch_t pWatch
    );

  pPosition_t GetPosition( const idPortfolio_t& idPortfolio, const std::string& sName );
  void UpdatePosition( const idPortfolio_t& idPortfolio, const std::string& sName ); // TODO: may not be funcfional
  void DeletePosition( const idPortfolio_t& idPortfolio, const std::string& sName );

  using OnPositionNeedsDetailsHandler = FastDelegate1<pPosition_t&>;
  void SetOnPositionNeedDetails( OnPositionNeedsDetailsHandler function ) {
    OnPositionNeedsDetails = function;
  }

  void PortfolioUpdateActive( pPortfolio_t );
  void PositionUpdateNotes( pPosition_t );

  ou::Delegate<pPortfolio_t&> OnPortfolioLoaded;
  ou::Delegate<pPortfolio_t&> OnPortfolioAdded;
  ou::Delegate<pPortfolio_t&> OnPortfolioUpdated;
  ou::Delegate<pPortfolio_t&> OnPortfolioDeleting;
  ou::Delegate<const idPortfolio_t&> OnPortfolioDeleted;

  ou::Delegate<pPosition_t&> OnPositionLoaded;
  ou::Delegate<pPosition_t&> OnPositionAdded;
  ou::Delegate<pPosition_t&> OnPositionUpdated;
  ou::Delegate<pPosition_t&> OnPositionDeleting;
  ou::Delegate<const idPosition_t&> OnPositionDeleted;

  template<class F> void ScanPortfolios( const idPortfolio_t& id, F );
  template<class F> void ScanPositions( mapPosition_t&, F );
  template<class F> void ScanPositions( const idPortfolio_t&, F );

  void LoadActivePortfolios();

  void AttachToSession( ou::db::Session* pSession );
  void DetachFromSession( ou::db::Session* pSession );

protected:

private:

  using mapPortfolio_pair_t = std::pair<idPortfolio_t, structPortfolio>;
  using mapPortfolios_t = std::map<idPortfolio_t, structPortfolio>;
  using mapPortfolios_iter_t = mapPortfolios_t::iterator;

  mapPortfolios_t m_mapPortfolios;

  // method for getting at child portfolios
  // portfolio id "" is root where all top level portfolios are linked
  using mapReportingPortfolios_t = std::map<idPortfolio_t,setPortfolioId_t>;
  using mapReportingPortfolios_pair_t = std::pair<idPortfolio_t,setPortfolioId_t>;
  mapReportingPortfolios_t m_mapReportingPortfolios;
  using iterReportingPortfolios_t = mapReportingPortfolios_t::iterator;

  void UpdateReportingPortfolio( idPortfolio_t idOwner, idPortfolio_t idReporting );

  OnPositionNeedsDetailsHandler OnPositionNeedsDetails;

  using fConstructPosition_t = std::function<pPosition_t()>;

  void ConstructPosition( // re-factored code
    const idPortfolio_t& idPortfolio, const std::string& sName,
    fConstructPosition_t&&
  );

  void PortfolioCommon( pPortfolio_t& );

  void HandleRegisterTables( ou::db::Session& session );
  void HandleRegisterRows( ou::db::Session& session );
  void HandlePopulateTables( ou::db::Session& session );
  void HandleLoadTables( ou::db::Session& sesion );

  void HandlePositionOnExecution( const Position& );
  void HandlePositionOnCommission( const Position& );

  void HandlePortfolioOnExecution( const Portfolio& );
  void HandlePortfolioOnCommission( const Portfolio& );

  void LoadPositions( const idPortfolio_t& idPortfolio, mapPosition_t& mapPosition );

};

template<class F> void PortfolioManager::ScanPortfolios( const idPortfolio_t& id,  F f ) {
  using namespace boost::adaptors;
  iterReportingPortfolios_t iter = m_mapReportingPortfolios.find( id );
  if ( m_mapReportingPortfolios.end() != iter ) {
    boost::for_each( iter->second, f );  // processes each reporting idPortfolio
  }
}

template<class F> void PortfolioManager::ScanPositions( mapPosition_t& mapPosition, F f ) {
  using namespace boost::adaptors;
  boost::for_each( mapPosition | map_values, f );
}

template<class F> void PortfolioManager::ScanPositions( const idPortfolio_t& idPortfolio, F f ) {
  using namespace boost::adaptors;
  pPortfolio_t pPortfolio = GetPortfolio( idPortfolio );  // ensure portfolio and positions are loaded
  mapPortfolios_iter_t iterPortfolio = m_mapPortfolios.find( idPortfolio );
  assert( m_mapPortfolios.end() != iterPortfolio );
  boost::for_each( iterPortfolio->second.mapPosition | map_values, f );
}

} // namespace tf
} // namespace ou
