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

#include <OUCharting/ChartDataView.h>

#include <TFTrading/Position.h>
#include <TFTrading/Portfolio.h>

#include <TFOptions/Chain.h>

#include "Leg.h"
#include "LegNote.h"
#include "SpreadSpecs.h"
#include "Exceptions.h"

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace option { // options

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

  using EColour = ou::Colour::enumColour;
  using EOptionSide = ou::tf::OptionSide::enumOptionSide;
  using EOrderSide = ou::tf::OrderSide::enumOrderSide;

  using fBuildPositionCallBack_t
    = std::function<void(pPosition_t,pChartDataView_t,EColour)>;
  using fBuildLeg_t
    = std::function<void(const idPortfolio_t&, EOptionSide, double, fBuildPositionCallBack_t&&)>;

  using fLegSelected_t = std::function<void(double /* spread */, double /* strike */, boost::gregorian::date, const std::string&)>;

  using pWatch_t      = ou::tf::option::Option::pWatch_t;
  using pOption_t     = ou::tf::option::Option::pOption_t;

  using fConstructedWatch_t  = std::function<void(pWatch_t)>;
  using fConstructedOption_t = std::function<void(pOption_t)>;

  using fConstructWatch_t  = std::function<void(const std::string&, fConstructedWatch_t&&)>;
  using fConstructOption_t = std::function<void(const std::string&, fConstructedOption_t&&)>;  // source from IQFeed Symbol Name

  using fActivateOption_t = std::function<void(pOption_t)>;
  using fOpenPosition_t = std::function<pPosition_t(Combo*,pOption_t,const std::string&)>; // string is Note from previous position
  using fDeactivateOption_t = std::function<void(pOption_t)>;

  enum class E20DayDirection { Unknown, Rising, Falling };

  enum class State { Initializing, Positions, Executing, Watching, Canceled, Closing };
  State m_state;

  // to be deprecated by converting to new style ChooseLegs
  using strike_pair_t = std::pair<double,double>; // higher, lower

  Combo( );
  Combo( Combo&& ); // needs experiementation on why no const works, const does not
  Combo( const Combo& ) = delete;
  Combo& operator=( const Combo& ) = delete;
  virtual ~Combo( );

  void Prepare(
    boost::gregorian::date date,
    const mapChains_t*,
    const SpreadSpecs&,
    fConstructOption_t&&,
    fActivateOption_t&&,
    fOpenPosition_t&&,
    fDeactivateOption_t&&
  );

  void SetPortfolio( pPortfolio_t );
  pPortfolio_t GetPortfolio() { return m_pPortfolio; }

  const LegNote::values_t& SetPosition( pPosition_t, pChartDataView_t pChartData, ou::Colour::enumColour );

  virtual void Tick( double dblUnderlyingSlope, double dblUnderlyingPrice, ptime dt );

  virtual void PlaceOrder( ou::tf::OrderSide::enumOrderSide, uint32_t nOrderQuantity ) = 0;
  virtual void PlaceOrder( ou::tf::OrderSide::enumOrderSide, uint32_t nOrderQuantity, LegNote::Type ) = 0;

  virtual double GetNet( double price );

  virtual void GoNeutral( boost::gregorian::date date, boost::posix_time::time_duration time ) {}
  virtual void AtClose() {}

  void CloseForProfits( double price );
  void TakeProfits( double price );
  void CloseExpiryItm( double price, const boost::gregorian::date date );
  void CloseFarItm( double price );
  bool CloseItmLeg( double price );
  bool CloseItmLegForProfit( double price );

  virtual void CancelOrders();
  void ClosePositions();  // TODO: adjust in superclass

  bool AreOrdersActive() const;
  void SaveSeries( const std::string& sPrefix );

  static citerChain_t SelectChain( const mapChains_t& mapChains, boost::gregorian::date date, boost::gregorian::days daysToExpiry );

protected:

  static const double m_dblTwentyPercent;
  static const double m_dblMaxStrikeDelta;
  static const double m_dblMaxStrangleDelta;

  fConstructOption_t m_fConstructOption;   // used in Collar
  fActivateOption_t m_fActivateOption;     // used in Combo
  fOpenPosition_t m_fOpenPosition;         // used in Collar
  fDeactivateOption_t m_fDeactivateOption; // used in Combo

  pPortfolio_t m_pPortfolio; // positions need to be associated with portfolio

  using mapLeg_t = std::map<LegNote::Type,ou::tf::Leg>;
  mapLeg_t m_mapLeg;

  virtual void Init( boost::gregorian::date date, const mapChains_t*, const SpreadSpecs& ) = 0;
  virtual void Init( LegNote::Type ) = 0;

  void DeactivatePositionOption( pPosition_t );

private:

};

} // namespace option
} // namespace tf
} // namespace ou

#endif /* COMBO_H */

