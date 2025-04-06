/************************************************************************
 * Copyright(c) 2025, One Unified. All rights reserved.                 *
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
 * File:      AppWebTrader.hpp
 * Author:    raymond@burkholder.net
 * Project:   WebTrader
 * Created:   2025/04/05 21:25:03
 */

#pragma once

#include <map>
#include <functional>

#include <boost/signals2.hpp>

#include <Wt/WEnvironment.h>
#include <Wt/WApplication.h>

#include "Server.hpp"

namespace Wt {
  class WTimer;
  class WButtonGroup;
  class WContainerWidget;
} // namespace Wt

class AppWebTrader: public Wt::WApplication {
public:

  using signalInternalPathChanged_t = boost::signals2::signal<void (Wt::WContainerWidget*)>;
  using slotInternalPathChanged_t = signalInternalPathChanged_t::slot_type;

  AppWebTrader( const Wt::WEnvironment& );
  virtual ~AppWebTrader( );

  virtual void initialize(); // Initializes the application, post-construction.
  virtual void finalize(); // Finalizes the application, pre-destruction.

  void RegisterPath( const std::string& sPath, const slotInternalPathChanged_t& slot );
  void UnRegisterPath( const std::string& sPath );

private:

  // TODO: set timer to update quotes, p/l, scale

  // TODO
  /*  2022-07-30, 07:39
      In Price to Buy/Sell, we currently have Market and Limit. Can you add SCALE. Behaviour will be like this.
        If this is Market - Buy/Sell at Market
        If it is Limit, allow user to enter Limit price
        If it is SCALE
          Display a popup window (for every row) and allow user to enter following:
            Starting Price
            Initial Quantity
            Incremental Quantity
            Increment/Decrement price

    hard validation on invesment overage?
  */
  using fTemplate_t = std::function<void(Wt::WContainerWidget*)>;

  using mapInternalPathChanged_t = std::map<const std::string, const slotInternalPathChanged_t>;
  mapInternalPathChanged_t m_mapInternalPathChanged;

  Server* m_pServer; // object managed by wt

  Wt::WTimer* m_timerLiveRefresh;

  // listed in rough order of usage on page
  Wt::WContainerWidget* m_pContainerDataEntry;
  Wt::WContainerWidget* m_pContainerDataEntryButtons;
  Wt::WContainerWidget* m_pContainerUnderlyingLiveData;
  Wt::WContainerWidget* m_pContainerTableEntry; // put this and next in ContainerTableEntry
  //Wt::WContainerWidget* m_pContainerTableEntryButtons;
  //Wt::WContainerWidget* m_pContainerNotifications;
  Wt::WContainerWidget* m_pContainerControl;

  using pButtonGroup_t = std::shared_ptr<Wt::WButtonGroup>;
  pButtonGroup_t m_pButtonGroupOption;
  pButtonGroup_t m_pButtonGroupSide;

  struct OptionAtStrike {
    bool m_bDrawn;
    Wt::WContainerWidget* m_pcw;
    OptionAtStrike( Wt::WContainerWidget* pcw ): m_bDrawn( false ), m_pcw( pcw ) {}
  };

  using mapOptionAtStrike_t = std::map<std::string,OptionAtStrike>;
  mapOptionAtStrike_t m_mapOptionAtStrike;

  using fUpdateStrikeSelection_t = std::function<void()>;
  fUpdateStrikeSelection_t m_fUpdateStrikeSelection;

  void AddLink( Wt::WContainerWidget*, const std::string& sClass, const std::string& sPath, const std::string& sAnchor );

  void HandleInternalPathChanged( const std::string& );
  void HandleInternalPathInvalid( const std::string& );

  void HandleLiveRefresh();

  void ShowMainMenu( Wt::WContainerWidget* );

  void HomeRoot( Wt::WContainerWidget* );
  void Home( Wt::WContainerWidget* );

  void ClearContainers(); // tbd

  void Containers_PrimaryDefine( Wt::WContainerWidget* ); // primary class containers
  void Container_UnderlyingPortfolio( Wt::WContainerWidget* );

  void Page_Template( Wt::WContainerWidget*, fTemplate_t );

  void Page_Login( Wt::WContainerWidget* );
  void Page_WatchList( Wt::WContainerWidget* );
  void Page_Blank( Wt::WContainerWidget* );
  void Page_SelectUnderlying( Wt::WContainerWidget* );
  void Page_LoadingUnderlyingAndChains_Load( Wt::WContainerWidget*, const std::string& sIQfeedUnderlyingName );
  void Page_SelectChainExpiry( Wt::WContainerWidget* ); // not consistent in container selection
  void Page_TableTrader( Wt::WContainerWidget*, const std::string& sDate );

  std::string ComposeOrderType(
    int ixOrderType,
    const std::string& strike,
    Wt::WLabel* pAsk,
    Wt::WLabel* pBid,
    Wt::WLineEdit* pLimitPrice,
    Wt::WLineEdit* pInitialQuantity,
    Wt::WLineEdit* IncrementQuantity,
    Wt::WLineEdit* pIncrementPrice
  );
};
