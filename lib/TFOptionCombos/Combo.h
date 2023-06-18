/************************************************************************
 * Copyright(c) 2019, One Unified. All rights reserved.                 *
 * email: info@oneunified.net                                           *
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

/*
 * File:    Combo.h
 * Author:  raymond@burkholder.net
 * Project: TFOptionCombos
 * Created on June 7, 2019, 5:08 PM
 */

#ifndef COMBO_H
#define COMBO_H

#include <map>
#include <unordered_set>

#include <OUCharting/ChartDataView.h>

#include <TFTrading/Position.h>
#include <TFTrading/Portfolio.h>
#include <TFTrading/Order_Combo.hpp>

#include <TFOptions/Chain.h>

#include "Leg.h"
#include "LegNote.h"
#include "Tracker.h"
#include "SpreadSpecs.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

namespace option { // options

// == ComboLeg

class ComboLeg { // migrating from CollarLeg
public:

  using fTest_t = std::function<bool(boost::posix_time::ptime,double,double)>; // underlying slope, price
  using vfTest_t = std::vector<fTest_t>; // do we need a vector of tests?  or just a single test?

  ou::tf::Leg m_leg;
  Tracker m_tracker;

  ComboLeg() = delete;
  ComboLeg( const ComboLeg& ) = delete;
  ComboLeg( ou::tf::Leg&& leg )
  : m_leg( std::move( leg ) ) {}
  ComboLeg( ComboLeg&& rhs )
  : m_leg( std::move( rhs.m_leg ) )
  {
    assert( rhs.m_vfTest.empty() );
  }
  ~ComboLeg() { m_vfTest.clear(); }

  void AddTest( fTest_t&& fTest ) {
    m_vfTest.emplace_back( std::move( fTest ) );
  }

  bool Test( boost::posix_time::ptime dt, double slope, double price ) {
    bool bRemove( false );
    for ( vfTest_t::value_type& fTest: m_vfTest ) {
      bRemove |= fTest( dt, slope, price );
    }
    return bRemove;
  }

protected:
private:

  vfTest_t m_vfTest; // functions to test & process leg

}; // ComboLeg

// == Combo

class Combo {  // TODO: convert to CRTP?
public:

  using idPortfolio_t = ou::tf::Portfolio::idPortfolio_t;

  using pInstrument_t = ou::tf::Instrument::pInstrument_t;
  using pPosition_t = ou::tf::Position::pPosition_t;
  using pPortfolio_t = ou::tf::Portfolio::pPortfolio_t;
  using pChartDataView_t = ou::ChartDataView::pChartDataView_t;

  using chain_t = ou::tf::option::Chain<ou::tf::option::chain::OptionName>;
  using mapChains_t = std::map<boost::gregorian::date, chain_t>;
  using citerChain_t = mapChains_t::const_iterator;

  using EColour = ou::Colour::EColour;
  using EOptionSide = ou::tf::OptionSide::EOptionSide;
  using EOrderSide = ou::tf::OrderSide::EOrderSide;

  using fBuildPositionCallBack_t
    = std::function<void(pPosition_t,pChartDataView_t,EColour)>;
  using fBuildLeg_t
    = std::function<void(const idPortfolio_t&, EOptionSide, double, fBuildPositionCallBack_t&&)>;

  using fLegSelected_t = std::function<void( double /* strike */, boost::gregorian::date, const std::string&)>;

  using pWatch_t      = ou::tf::option::Option::pWatch_t;
  using pOption_t     = ou::tf::option::Option::pOption_t;

  using fConstructedWatch_t  = std::function<void(pWatch_t)>;
  using fConstructedOption_t = std::function<void(pOption_t)>;

  using fConstructWatch_t  = std::function<void(const std::string&, fConstructedWatch_t&&)>;
  using fConstructOption_t = std::function<void(const std::string&, fConstructedOption_t&&)>;  // source from IQFeed Symbol Name

  using pOrderCombo_t = ou::tf::OrderCombo::pOrderCombo_t;

  // TODO: ActivateOption to return lambda to update menu
  using fMenuActivation_t = std::function<void()>;
  struct MenuActivation {
    std::string sLabel;
    fMenuActivation_t fMenuActivation;

    MenuActivation() = delete;
    MenuActivation( MenuActivation& rhs ) = delete;
    MenuActivation( const MenuActivation& rhs ) //= delete; // compiler needs it this way for OptionsRepository::Add
    : sLabel( std::move( rhs.sLabel ) ), fMenuActivation( std::move( rhs.fMenuActivation ) ) {}
    MenuActivation( MenuActivation&& rhs )
    : sLabel( std::move( rhs.sLabel ) ), fMenuActivation( std::move( rhs.fMenuActivation ) ) {}
    MenuActivation( const std::string& sLabel_, fMenuActivation_t&& f )
    : sLabel( sLabel_ ), fMenuActivation( std::move( f ) ) {}
  };
  using vMenuActivation_t = std::vector<MenuActivation>;
  using fOptionRegistryAdd_t = std::function<void(pOption_t)>;
  using fActivateOption_t = std::function<void(pOption_t,const std::string& legname, vMenuActivation_t&& )>;
  using fConstructPosition_t = std::function<pPosition_t(Combo*,pOption_t,const std::string&)>; // string is Note from previous position
  using fDeactivateOption_t = std::function<void(pOption_t)>;
  using fOptionRegistryRemove_t = std::function<void(pOption_t)>;

  enum class E20DayDirection { Unknown, Rising, Falling };

  Combo();
  Combo( Combo&& );
  Combo( const Combo& ) = delete;
  Combo& operator=( const Combo& ) = delete;
  virtual ~Combo(); // may no longer require 'virtual'

  void Prepare(
    boost::gregorian::date date
  , const mapChains_t*
  , const SpreadSpecs&
  , fConstructOption_t&&
  , fOptionRegistryAdd_t&&
  , fActivateOption_t&&
  , fConstructPosition_t&&
  , fDeactivateOption_t&&
  , fOptionRegistryRemove_t&&
  );

  void SetPortfolio( pPortfolio_t );
  pPortfolio_t GetPortfolio() { return m_pPortfolio; }

  // TODO: ensure options/positions are not duplicated
  //   ie possible SynthLong overlapping a DltaPlsGmPls
  const LegNote::values_t& AddPosition( pPosition_t );
  void SetChartData( pChartDataView_t pChartData, ou::Colour::EColour );
  void DelChartData();

  void Tick( double dblUnderlyingSlope, double dblUnderlyingPrice, ptime dt );

  virtual double GetNet( double price );

  void GoNeutral( boost::gregorian::date date, boost::posix_time::time_duration time );
  void AtClose();

  void CloseForProfits( double price );
  void TakeProfits( double price );
  void CloseExpiryItm( double price, const boost::gregorian::date date );
  void CloseFarItm( double price );
  bool CloseItmLeg( double price );
  bool CloseItmLegForProfit( double price );

  void CancelOrders();
  void ClosePositions();  // TODO: adjust in superclass

  bool AreOrdersActive() const;
  void SaveSeries( const std::string& sPrefix );

  void Submit( pOrderCombo_t, const std::string& sComment );

protected:
private:

  fConstructOption_t m_fConstructOption;
  fOptionRegistryAdd_t m_fOptionRegistryAdd;
  fActivateOption_t m_fActivateOption;
  fConstructPosition_t m_fConstructPosition;
  fDeactivateOption_t m_fDeactivateOption;
  fOptionRegistryRemove_t m_fOptionRegistryRemove;

  pPortfolio_t m_pPortfolio; // positions need to be associated with portfolio

  // use lambdas to maintain local iterator for specific call types
  //   eg, in the vfTest lambda, then can locally access tracker/monitor/etc

  using mapComboLeg_t = std::multimap<LegNote::Type,ComboLeg>;
  mapComboLeg_t m_mapComboLeg;

  // contains active ComboOrders
  using setpOrderCombo_t = std::unordered_set<pOrderCombo_t>;
  setpOrderCombo_t m_setpOrderCombo_Active;

  // contains completed ComboOrders for cleanup
  // does this require a mutex? used for removing completed orders?
  using vOrderComboIter_t = std::vector<setpOrderCombo_t::iterator>;
  vOrderComboIter_t m_vOrderComboIter_CleanUp;

  using fInitTrackOption_t = std::function<void(ComboLeg&)>;
  using mapInitTrackOption_t = std::map<LegNote::Type,fInitTrackOption_t>;
  mapInitTrackOption_t m_mapInitTrackOption;

  citerChain_t m_iterChainFront;
  citerChain_t m_iterChainBack;

  enum class ENeutral {
    no_leg        // initial - no legs available
  , find_leg_one_call  // load leg and wait for greeks
  , find_leg_one_put  // load leg and wait for greeks
  , find_leg_two_call  // load leg and wait for greeks
  , find_leg_two_put  // load leg and wait for greeks
  , search_in_call        // shift legs to surround current delta
  , search_in_put        // shift legs to surround current delta
  , stable        // current delta is between the two legs
  } m_stateNeutral;

  pOption_t m_pOptionNeutralCandidateHigh;
  pOption_t m_pOptionNeutralCandidateLow;

  void InitTracker(
    ComboLeg&,
    citerChain_t
  );

  void InitTrackLongOption(
    ComboLeg&,
    citerChain_t
    );

  void InitTrackShortOption(
    ComboLeg&,
    citerChain_t
    );

  void CalendarRoll( ComboLeg& );
  void DiagonalRoll( ComboLeg& );
  void LegClose( ComboLeg& );

  void DeactivatePositionOption( pPosition_t );

  pChartDataView_t m_pChartDataView;

  ou::ChartEntryIndicator m_ceProfitLoss;

  ou::ChartEntryIndicator m_ceImpliedVolatility;
  ou::ChartEntryIndicator m_ceDelta;
  ou::ChartEntryIndicator m_ceGamma;
  ou::ChartEntryIndicator m_ceTheta;
  ou::ChartEntryIndicator m_ceVega;

  void Init( boost::gregorian::date, const mapChains_t*, const SpreadSpecs& );

  void PositionNote( pPosition_t&, LegNote::State );

  void NeutralCandidate( double price, double delta, double gamma );

};

// TODO:
//  one time only leg use:
//     do not overwrite a Leg,
//     instead, create a new leg, perform an entry, and close, then remove old leg
//     this allows a margin neutral roll - will need a multi-leg basket for this

} // namespace option
} // namespace tf
} // namespace ou

#endif /* COMBO_H */

