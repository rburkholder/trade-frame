/************************************************************************
 * Copyright(c) 2022, One Unified. All rights reserved.                 *
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
 * File:    OptionStatistics.hpp
 * Author:  raymond@burkholder.net
 * Project: BasketTrading
 * Created: April 24, 2022 16:07
 */

#include <OUCharting/ChartDataView.h>
#include <OUCharting/ChartEntryVolume.h>
#include <OUCharting/ChartEntryIndicator.h>

#include <TFOptions/Option.h>
#include <TFTrading/Position.h>

namespace ou {
namespace tf {
  class TreeItem;
}
}

class OptionStatistics {
public:

  using pWatch_t     = ou::tf::option::Option::pWatch_t;
  using pOption_t    = ou::tf::option::Option::pOption_t;
  using pPosition_t  = ou::tf::Position::pPosition_t;

  using pChartDataView_t =ou::ChartDataView::pChartDataView_t;

  OptionStatistics( pOption_t pOption );
  ~OptionStatistics();

  using pOptionStatistics_t = std::shared_ptr<OptionStatistics>;
  static pOptionStatistics_t Factory( pOption_t pOption ) { return std::make_shared<OptionStatistics>( pOption ); }

  pOption_t Option() { return m_pOption; }
  pChartDataView_t ChartDataView() { return m_pdvChart; }

  void Set( pPosition_t pPosition ) { m_pPosition = pPosition; }
  void Set( ou::tf::TreeItem* pti );

protected:
private:

  enum ChartSlot { Price, Volume, Spread, PL, IV, Delta, Gamma, Theta, Rho, Vega };

  pOption_t m_pOption;
  pPosition_t m_pPosition;

  ou::tf::TreeItem* m_ptiSelf;

  pChartDataView_t m_pdvChart; // the data

  ou::ChartEntryIndicator m_ceAsk;
  ou::ChartEntryIndicator m_ceTrade;
  ou::ChartEntryIndicator m_ceBid;

  ou::ChartEntryVolume m_ceVolume;

  ou::ChartEntryVolume m_ceAskVolume;
  ou::ChartEntryVolume m_ceBidVolume;

  ou::ChartEntryIndicator m_ceSpread;

  ou::ChartEntryIndicator m_cePLTotal;

  ou::ChartEntryIndicator m_ceImpliedVolatility;
  ou::ChartEntryIndicator m_ceDelta;
  ou::ChartEntryIndicator m_ceGamma;
  ou::ChartEntryIndicator m_ceTheta;
  ou::ChartEntryIndicator m_ceRho;
  ou::ChartEntryIndicator m_ceVega;

  void HandleQuote( const ou::tf::Quote& quote );
  void HandleTrade( const ou::tf::Trade& trade );
  void HandleGreek( const ou::tf::Greek& greek );

  ou::tf::TreeItem* GetTreeItem() { return m_ptiSelf; } // not used at the moment

};
