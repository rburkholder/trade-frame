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
 * File:      AppTableTrader.hpp
 * Author:    raymond@burkholder.net
 * Created:   2022/08/02 13:42:30
 */

#pragma once

#include <map>
#include <functional>

#include <boost/signals2.hpp>

#include <Wt/WEnvironment.h>
#include <Wt/WApplication.h>

#include "Server.h"

class AppTableTrader: public Wt::WApplication {
public:

  using signalInternalPathChanged_t = boost::signals2::signal<void (Wt::WContainerWidget*)>;
  using slotInternalPathChanged_t = signalInternalPathChanged_t::slot_type;

  AppTableTrader( const Wt::WEnvironment& );
  virtual ~AppTableTrader( );

  virtual void initialize(); // Initializes the application, post-construction.
  virtual void finalize(); // Finalizes the application, pre-destruction.

  void RegisterPath( const std::string& sPath, const slotInternalPathChanged_t& slot );
  void UnRegisterPath( const std::string& sPath );

private:

  using fTemplate_t = std::function<void(Wt::WContainerWidget*)>;

  using mapInternalPathChanged_t = std::map<const std::string, const slotInternalPathChanged_t>;
  mapInternalPathChanged_t m_mapInternalPathChanged;

  Server* m_pServer; // object managed by wt

  void AddLink( Wt::WContainerWidget*, const std::string& sClass, const std::string& sPath, const std::string& sAnchor );

  void HandleInternalPathChanged( const std::string& );
  void HandleInternalPathInvalid( const std::string& );

  void ShowMainMenu( Wt::WContainerWidget* );

  void HomeRoot( Wt::WContainerWidget* );
  void Home( Wt::WContainerWidget* );

  void TemplatePage( Wt::WContainerWidget*, fTemplate_t );

  void LoginPage( Wt::WContainerWidget* );
  void ActionPage( Wt::WContainerWidget* );
};
