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

  typedef Portfolio::pPortfolio_t pPortfolio_t;
  typedef keytypes::idPortfolio_t idPortfolio_t;
  typedef std::set<idPortfolio_t> setPortfolioId_t;

  typedef Portfolio::EPortfolioType EPortfolioType;
  typedef Portfolio::currency_t currency_t;

  typedef Position::pPosition_t pPosition_t;
  typedef keytypes::idPosition_t idPosition_t;
  typedef keytypes::idAccount_t idAccount_t;
  typedef keytypes::idInstrument_t idInstrument_t;

  typedef keytypes::idAccountOwner_t idAccountOwner_t;

  typedef Instrument::pInstrument_cref pInstrument_cref;
  typedef Position::pProvider_t pProvider_t;

  typedef std::pair<const Position&, const Execution&> execution_pair_t;
  typedef const execution_pair_t& execution_delegate_t;

  typedef std::pair<std::string, pPosition_t> mapPosition_pair_t;
  typedef std::map<std::string, pPosition_t> mapPosition_t;
  typedef mapPosition_t::iterator mapPosition_iter_t;

  struct structPortfolio {
    pPortfolio_t pPortfolio;
    mapPosition_t mapPosition;
    structPortfolio( pPortfolio_t& pPortfolio_ ) : pPortfolio( pPortfolio_ ) {};
  };

  PortfolioManager(void) {};
  ~PortfolioManager(void) {};

  pPortfolio_t ConstructPortfolio( 
    const idPortfolio_t& idPortfolio, const idAccountOwner_t& idAccountOwner, const idPortfolio_t& idOwner, 
    EPortfolioType ePortfolioType, currency_t eCurrency, const std::string& sDescription = "" );
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

  ou::Delegate<const idPortfolio_t&> OnPortfolioAdded;
  ou::Delegate<const idPortfolio_t&> OnPortfolioUpdated;
  ou::Delegate<const idPortfolio_t&> OnPortfolioDeleted;

  ou::Delegate<const idPosition_t&> OnPositionAdded;
  ou::Delegate<const idPosition_t&> OnPositionUpdated;
  ou::Delegate<const idPosition_t&> OnPositionDeleted;

  template<class F> void ScanPortfolios( F, const idPortfolio_t& id = "" );
  template<class F> void ScanPositions( mapPosition_t&, F );
  template<class F> void ScanPositions( const idPortfolio_t&, F );

  void LoadActivePortfolios( void );

  void AttachToSession( ou::db::Session* pSession );
  void DetachFromSession( ou::db::Session* pSession );

protected:

private:

  typedef std::pair<idPortfolio_t, structPortfolio> mapPortfolio_pair_t;
  typedef std::map<idPortfolio_t, structPortfolio> mapPortfolios_t;
  typedef mapPortfolios_t::iterator mapPortfolios_iter_t;

  mapPortfolios_t m_mapPortfolios;

  // method for getting at child portfolios
  // portfolio id "" is root where all top level portfolios are linked
  typedef std::map<idPortfolio_t,setPortfolioId_t> mapReportingPortfolios_t;
  typedef std::pair<idPortfolio_t,setPortfolioId_t> mapReportingPortfolios_pair_t;
  mapReportingPortfolios_t m_mapReportingPortfolios;
  typedef mapReportingPortfolios_t::iterator iterReportingPortfolios_t;

  void UpdateReportingPortfolio( idPortfolio_t idOwner, idPortfolio_t idReporting );

  OnPositionNeedsDetailsHandler OnPositionNeedsDetails;

  void HandleRegisterTables( ou::db::Session& session );
  void HandleRegisterRows( ou::db::Session& session );
  void HandlePopulateTables( ou::db::Session& session );

  void HandlePositionOnExecution( const Position& );
  void HandlePositionOnCommission( const Position& );

  void HandlePortfolioOnExecution( const Portfolio& );
  void HandlePortfolioOnCommission( const Portfolio& );

  void LoadPositions( const idPortfolio_t& idPortfolio, mapPosition_t& mapPosition );

};

template<class F> void PortfolioManager::ScanPortfolios( F f, const idPortfolio_t& id ) {
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
