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
 * File:      AppTableTrader.cpp
 * Author:    raymond@burkholder.net
 * Created:   2022/08/02 13:42:30
 */

#include <boost/log/trivial.hpp>

#include <Wt/WText.h>
#include <Wt/WLabel.h>
#include <Wt/WAnchor.h>
#include <Wt/WLineEdit.h>
//#include <Wt/WTextArea.h>
#include <Wt/WGroupBox.h>
#include <Wt/WPushButton.h>
#include <Wt/WButtonGroup.h>
#include <Wt/WRadioButton.h>
#include <Wt/WSelectionBox.h>
#include <Wt/WContainerWidget.h>

#include "AppTableTrader.hpp"

namespace {
  static const std::string sTitle( "Table Trader" );
}

// https://www.webtoolkit.eu/wt/doc/reference/html/classWt_1_1WApplication.html

AppTableTrader::AppTableTrader( const Wt::WEnvironment& env )
: Wt::WApplication( env ),
  m_pServer( dynamic_cast<Server*>( env.server() ) )
{

  useStyleSheet("style/tt.css");

  internalPathChanged().connect( this, &AppTableTrader::HandleInternalPathChanged );
  internalPathInvalid().connect( this, &AppTableTrader::HandleInternalPathInvalid );

  namespace ph = std::placeholders;
  RegisterPath( "/", std::bind( &AppTableTrader::HomeRoot, this, ph::_1 ) );

  struct callback {
    void operator()( Wt::WContainerWidget* pcw ) {}
  };

  enableUpdates(); // enable updates of async data

  TemplatePage(
    root(),
    [this](Wt::WContainerWidget* pcw){
      //LoginPage( pcw );
      ActionPage( pcw ); // skip the login for now
    } );

}

AppTableTrader::~AppTableTrader( ) { }

void AppTableTrader::initialize() {
  Wt::WApplication::log( "info" ) << "AppTableTrader::initialize()";
}

void AppTableTrader::finalize() {
  Wt::WApplication::log( "info" ) << "AppTableTrader::finalize()";
}

void AppTableTrader::HandleInternalPathChanged( const std::string& sPath ) {
  root()->clear();
  std::string sMessage;
  sMessage += "HandleInternalPathChanged: ";
  sMessage += sPath;
  mapInternalPathChanged_t::const_iterator iter = m_mapInternalPathChanged.find( sPath );
  if ( m_mapInternalPathChanged.end() != iter ) {
    sMessage += "iter";
    iter->second( root() );
  }
  else {
    // default home page, or error page, and register a default page
    sMessage += "root";
    Home( root() );
  }
  Wt::WApplication::log( "info" ) << sMessage;
}

void AppTableTrader::HandleInternalPathInvalid( const std::string& s ) {
  Wt::WApplication::log( "warn" ) << "*** HandleInternalPathInvalid: " << s;
}

void AppTableTrader::RegisterPath( const std::string& sPath, const slotInternalPathChanged_t& slot ) {
  mapInternalPathChanged_t::const_iterator iter = m_mapInternalPathChanged.find( sPath );
  if ( m_mapInternalPathChanged.end() != iter )
    std::runtime_error( "path exists" );
  m_mapInternalPathChanged.insert( mapInternalPathChanged_t::value_type( sPath, slot ) );
}

void AppTableTrader::UnRegisterPath( const std::string& sPath ) {
  mapInternalPathChanged_t::const_iterator iter = m_mapInternalPathChanged.find( sPath );
  if ( m_mapInternalPathChanged.end() == iter )
    std::runtime_error( "path not found" );
  m_mapInternalPathChanged.erase( iter );
}

void AppTableTrader::AddLink( Wt::WContainerWidget* pcw, const std::string& sClass, const std::string& sPath, const std::string& sAnchor ) {
  Wt::WContainerWidget* pContainer
    = pcw->addWidget( std::make_unique<Wt::WContainerWidget>() );

  pContainer->setStyleClass( sClass );

  // https://www.webtoolkit.eu/wt/doc/reference/html/classWt_1_1WAnchor.html
  Wt::WLink link( Wt::LinkType::InternalPath, sPath );

  Wt::WAnchor* pAnchor
    = pContainer->addWidget( std::make_unique<Wt::WAnchor>( link, sAnchor ) );

  // WApplication::instance()->internalPathChanged().connect(this, &DocsListWidget::onInternalPathChange);
}

void AppTableTrader::ShowMainMenu( Wt::WContainerWidget* pcw ) {

  // still need to show the widget, and there needs to be a click event so when
  //  logged in we can refresh
  // so devote a page to the login, clean out main menu,
  // but still need an event on when logged in is successful
  //   because nothing else happens on the page
/*
  if ( m_pAuth->LoggedIn() ) {

    AddLink( pcw, "member", "/show/addresses", "Address List" );
    AddLink( pcw, "member", "/admin/tables/upload", "Upload" );

    // <a id="ov7qcp1"
    //    href="admin/tables/populate/mysql?wtd=jLpA57e4vgIIoYxI"
    //    class="Wt-rr"><span id="ov7qcp0">Populate Tables: MySQL sourced</span>
    //    </a>
    auto pMenu = new Wt::WContainerWidget( pcw );
    // test against database, and figure out which can be shown, particularily the tables/init one
    pMenu->setList(true); // ==> sub WContainerWidget added as <li> elements
    AddLink( pMenu, "admin", "/admin/tables/init", "Init Tables" );
    AddLink( pMenu, "admin", "/admin/tables/populate/basics", "Populate Tables: Basics" );
    // use the Upload class to do this one:
    AddLink( pMenu, "admin", "/ad min/tables/populate/mysql",  "Populate Tables: MySQL sourced" );
    // use the Upload class to do this one:
    AddLink( pMenu, "admin", "/admin/tables/populate/smcxml", "Populate Tables: SMC XML sourced" );

  }
  else {

  }
  */

  //pcw->addWidget( std::make_unique<page::TimeRecord>( m_session ) );

}

void AppTableTrader::HomeRoot( Wt::WContainerWidget* pcw ) {
  //std::cout << "root home" << std::endl;

  namespace ph = std::placeholders;
  TemplatePage( pcw, std::bind( &AppTableTrader::ShowMainMenu, this, ph::_1 ) );
}

void AppTableTrader::Home( Wt::WContainerWidget* pcw ) {

  namespace ph = std::placeholders;
  TemplatePage( pcw, std::bind( &AppTableTrader::ShowMainMenu, this, ph::_1 ) );
}

void AppTableTrader::TemplatePage(Wt::WContainerWidget* pcw, fTemplate_t f) {

  setTitle( sTitle );

  //auto title = pcw->addWidget( std::make_unique<Wt::WText>( sTitle ) );

  //pcw->addWidget( std::make_unique<Wt::WBreak>() );

  //auto pTitle( new Wt::WText( sTitle ) );
  //pTitle->setStyleClass( "MainTitle" );

  //pcw->addWidget( pTitle );

    // should show up to the right of the title
  //if ( this->internalPathMatches( "/auth/signin" ) ) {
    // don't show sign in status
  //}
  //else {
//    if ( m_pAuth->LoggedIn() ) {
//      //pcw->addWidget( m_pAuth->NewAuthWidget() );
//      AddLink( pcw, "member", "/auth/signout", "Sign Out" );
//      AddLink( pcw, "member", "/member/home", "Home" );
//    }
//    else {
//      AddLink( pcw, "admin", "/auth/signin", "Sign In" );
//      AddLink( pcw, "default", "/home", "Home" );
//    }

//  }

  f( pcw );
}

void AppTableTrader::LoginPage( Wt::WContainerWidget* pcw ) {

  // TODO: will need to restore state if Server/Server_impl has something running

  Wt::WContainerWidget* pContainerLoginFrame = pcw->addWidget( std::make_unique<Wt::WContainerWidget>() );
  pContainerLoginFrame->addStyleClass( "classInputRow" );

    Wt::WContainerWidget* pContainerTitle = pContainerLoginFrame->addWidget( std::make_unique<Wt::WContainerWidget>() );

    Wt::WContainerWidget* pContainerFields = pContainerLoginFrame->addWidget( std::make_unique<Wt::WContainerWidget>() );

      Wt::WContainerWidget* pContainerLoginUserName = pContainerFields->addWidget( std::make_unique<Wt::WContainerWidget>() );
      Wt::WLabel* pLabelUserName = pContainerLoginUserName->addWidget( std::make_unique<Wt::WLabel>( "UserName: " ) );
      Wt::WLineEdit* pEditUserName = pContainerLoginUserName->addWidget( std::make_unique<Wt::WLineEdit>() );
      pLabelUserName->setBuddy( pEditUserName );

      Wt::WContainerWidget* pContainerLoginPassword = pContainerFields->addWidget( std::make_unique<Wt::WContainerWidget>() );
      Wt::WLabel* pLabelPassWord = pContainerLoginPassword->addWidget( std::make_unique<Wt::WLabel>( "Password: " ) );
      Wt::WLineEdit* pEditPassWord = pContainerLoginPassword->addWidget( std::make_unique<Wt::WLineEdit>() );
      pEditPassWord->setEchoMode( Wt::EchoMode::Password );
      pLabelPassWord->setBuddy( pEditPassWord );

    Wt::WContainerWidget* pContainerButtons = pContainerLoginFrame->addWidget( std::make_unique<Wt::WContainerWidget>() );

      Wt::WPushButton *pbtnLogin = pContainerButtons->addWidget( std::make_unique<Wt::WPushButton>( "Login" ) );

    Wt::WContainerWidget* pContainerNotification = pContainerLoginFrame->addWidget( std::make_unique<Wt::WContainerWidget>() );

  pEditUserName->setFocus();

  pbtnLogin->clicked().connect(
    [this,
      pbtnLogin,
      pEditUserName, pEditPassWord,
      pContainerNotification
    ](){

      pContainerNotification->clear();
      bool bOk( true );

      const std::string sUserName = pEditUserName->text().toUTF8();
      if ( 0 == sUserName.size() ) {
        Wt::WText* pText = pContainerNotification->addWidget( std::make_unique<Wt::WText>( "UserName: required" ) );
        pText->addStyleClass( "classErrorMessage" );
        bOk = false;
      }

      const std::string sPassWord = pEditPassWord->text().toUTF8();
      if ( 0 == sPassWord.size() ) {
        Wt::WText* pText = pContainerNotification->addWidget( std::make_unique<Wt::WText>( "PassWord: required" ) );
        pText->addStyleClass( "classErrorMessage" );
        bOk = false;
      }

      if ( bOk ) {

        pEditUserName->setEnabled( false );
        pEditPassWord->setEnabled( false );

        bOk = m_pServer->ValidateLogin( sUserName, sPassWord );

        if ( bOk ) {
          TemplatePage(
            root(),
            [this]( Wt::WContainerWidget* pcw ){
              ActionPage( pcw );
            });
        }
        else {

          pEditUserName->setEnabled( true );
          pEditPassWord->setEnabled( true );

          Wt::WText* pText = pContainerNotification->addWidget( std::make_unique<Wt::WText>( "UserName/PassWord: no match" ) );
          pText->addStyleClass( "classErrorMessage" );
          bOk = false;

        }
      }

    });
}

void AppTableTrader::ActionPage( Wt::WContainerWidget* pcw ) {

  pcw->clear();

  // TODO: will need to load pre-existing state

  m_pContainerDataEntry = pcw->addWidget( std::make_unique<Wt::WContainerWidget>() );
    Wt::WContainerWidget* pContainerUnderlying = m_pContainerDataEntry->addWidget( std::make_unique<Wt::WContainerWidget>() );
      Wt::WLabel* pLabelUnderlying = pContainerUnderlying->addWidget( std::make_unique<Wt::WLabel>( "Underlying: " ) );
      Wt::WSelectionBox* pSelectUnderlying = pContainerUnderlying->addWidget( std::make_unique<Wt::WSelectionBox>() );
      pSelectUnderlying->setSelectionMode( Wt::SelectionMode::Single );
      pSelectUnderlying->setVerticalSize( 2 );
      pLabelUnderlying->setBuddy( pSelectUnderlying );
      m_pServer->AddCandidateFutures(
        [pSelectUnderlying]( const std::string& sUnderlyingFuture ){
          pSelectUnderlying->addItem( sUnderlyingFuture );
        });
  m_pContainerDataEntryButtons = pcw->addWidget( std::make_unique<Wt::WContainerWidget>() );
  m_pContainerLiveData = pcw->addWidget( std::make_unique<Wt::WContainerWidget>() );

    // TODO: put into own sub-container
    Wt::WLabel* pLabelUnderlyingLabel = m_pContainerLiveData->addWidget( std::make_unique<Wt::WLabel>( "Underlying: " ) );
    Wt::WLabel* pLabelUnderlyingName  = m_pContainerLiveData->addWidget( std::make_unique<Wt::WLabel>() );
    Wt::WLabel* pLabelMultiplierLabel = m_pContainerLiveData->addWidget( std::make_unique<Wt::WLabel>( "Multiplier: " ) );
    Wt::WLabel* pLabelMultiplierValue = m_pContainerLiveData->addWidget( std::make_unique<Wt::WLabel>() );

    Wt::WLabel* pLabel = m_pContainerLiveData->addWidget( std::make_unique<Wt::WLabel>( "Current Price: " ) );
    Wt::WLabel* pLivePrice = m_pContainerLiveData->addWidget( std::make_unique<Wt::WLabel>( "" ) );
  m_pContainerTableEntry = pcw->addWidget( std::make_unique<Wt::WContainerWidget>() );
  m_pContainerTableEntryButtons = pcw->addWidget( std::make_unique<Wt::WContainerWidget>() );
  m_pContainerNotifications = pcw->addWidget( std::make_unique<Wt::WContainerWidget>() );
  m_pContainerControl = pcw->addWidget( std::make_unique<Wt::WContainerWidget>() );
    //Wt::WRadioButton* pRadioButtonStop = pContainerControl->addWidget( std::make_unique<Wt::WRadioButton>( "Stop" ) );

  //pRadioButtonStop->checked().connect( // does not work due to Wt::WServer::waitForShutdown();
  //  [this](){
  //    m_pServer->stop();
  //  }
  //);

  pSelectUnderlying->activated().connect(
    [this,pSelectUnderlying,pLivePrice,pLabelUnderlyingName,pLabelMultiplierValue](){
      pSelectUnderlying->setEnabled( false );
      std::string sUnderlying = pSelectUnderlying->valueText().toUTF8();

      m_pContainerDataEntry->clear();

      Wt::WContainerWidget* pContainerExpiries = m_pContainerDataEntry->addWidget( std::make_unique<Wt::WContainerWidget>() );
        Wt::WLabel* pLabelExpiries = pContainerExpiries->addWidget( std::make_unique<Wt::WLabel>( "Option Chain Expiries: " ) );
        Wt::WSelectionBox* pSelectExpiries = pContainerExpiries->addWidget( std::make_unique<Wt::WSelectionBox>() );
        pSelectExpiries->setSelectionMode( Wt::SelectionMode::Single );
        pSelectExpiries->setVerticalSize( 3 );
        pLabelExpiries->setBuddy( pSelectExpiries );

      m_pServer->Start(
        sessionId(), sUnderlying,
        [this,pLabelUnderlyingName,pLabelMultiplierValue]( const std::string& sName, const std::string& sMultiplier ){ // fUpdateUnderlyingInfo_t
          pLabelUnderlyingName->setText( sName );
          pLabelMultiplierValue->setText( sMultiplier );
          triggerUpdate();
        },
        [this,pLivePrice]( const std::string& sPrice ){ // fUpdateUnderlyingPrice_t
          pLivePrice->setText( sPrice );
          triggerUpdate(); // TODO: trigger on timer to reduce traffic
        },
        [pSelectExpiries]( const std::string& sDate ){ // fUpdateOptionExpiries_t
          // TODO: implement timer to indicate duration
          pSelectExpiries->addItem( sDate );
        },
        [this,pSelectExpiries](){ // fUpdateOptionExpiriesDone_t
          // TODO: disable once filled
          pSelectExpiries->activated().connect(
            [this,pSelectExpiries](){
              pSelectExpiries->setEnabled( false );
              std::string sDate = pSelectExpiries->valueText().toUTF8();
              m_pContainerDataEntry->clear();

              enum class EOption { call = 1, put = 2 };
              {
                auto containerOption = m_pContainerDataEntry->addWidget( std::make_unique<Wt::WGroupBox>("Call/Put") );
                m_pButtonGroupOption = std::make_shared<Wt::WButtonGroup>();
                Wt::WRadioButton* btnCall = containerOption->addWidget( std::make_unique<Wt::WRadioButton>( "Call" ) );
                //container->addWidget(std::make_unique<Wt::WBreak>());
                m_pButtonGroupOption->addButton(btnCall, (int)EOption::call );
                Wt::WRadioButton* btnPut = containerOption->addWidget( std::make_unique<Wt::WRadioButton>( "Put" ) );
                //container->addWidget(std::make_unique<Wt::WBreak>());
                m_pButtonGroupOption->addButton( btnPut, (int)EOption::put );
                m_pButtonGroupOption->setCheckedButton( m_pButtonGroupOption->button( (int)EOption::call ) );
              }

              enum class ESide { buy = 1, sell = 2 };
              {
                auto containerSide = m_pContainerDataEntry->addWidget( std::make_unique<Wt::WGroupBox>("Buy/Sell") );
                m_pButtonGroupSide = std::make_shared<Wt::WButtonGroup>();
                Wt::WRadioButton* btnBuy = containerSide->addWidget( std::make_unique<Wt::WRadioButton>( "Buy" ) );
                //container->addWidget(std::make_unique<Wt::WBreak>());
                m_pButtonGroupSide->addButton( btnBuy, (int)ESide::buy );
                Wt::WRadioButton* btnSell = containerSide->addWidget( std::make_unique<Wt::WRadioButton>( "Sell" ) );
                //container->addWidget(std::make_unique<Wt::WBreak>());
                m_pButtonGroupSide->addButton( btnSell, (int)ESide::sell );
                m_pButtonGroupSide->setCheckedButton( m_pButtonGroupSide->button( (int)ESide::buy ) );
              }

              Wt::WLabel* pLabelInvestment = m_pContainerDataEntry->addWidget( std::make_unique<Wt::WLabel>( "Investment: " ) );
              Wt::WLineEdit* pWLineEditInvestment = m_pContainerDataEntry->addWidget( std::make_unique<Wt::WLineEdit>() );
              pLabelInvestment->setBuddy( pWLineEditInvestment );
              pWLineEditInvestment->setText( "100000" );

              Wt::WLabel* pLabelStrikes = m_pContainerDataEntry->addWidget( std::make_unique<Wt::WLabel>( "Strikes: " ) );
              Wt::WSelectionBox* pSelectStrikes = m_pContainerDataEntry->addWidget( std::make_unique<Wt::WSelectionBox>() );
              pSelectStrikes->setSelectionMode( Wt::SelectionMode::Extended );
              pSelectStrikes->setVerticalSize( 10 );
              pLabelStrikes->setBuddy( pSelectStrikes );

              m_pServer->PrepareStrikeSelection(
                sDate,
                [pSelectStrikes](const std::string& sStrike){ // fPopulateStrike_t
                  pSelectStrikes->addItem( sStrike );
                },
                [this,pSelectStrikes](){ // fPopulateStrikeDone_t
                  Wt::WPushButton* pBtnCancelAll = m_pContainerTableEntryButtons->addWidget( std::make_unique<Wt::WPushButton>( "Cancel All" ) );
                  Wt::WPushButton* pBtnCloseAll = m_pContainerTableEntryButtons->addWidget( std::make_unique<Wt::WPushButton>( "Close All" ) );
                  Wt::WPushButton* pBtnStart = m_pContainerTableEntryButtons->addWidget( std::make_unique<Wt::WPushButton>( "Start" ) );
                  pSelectStrikes->changed().connect( // only this one works with multiple selection
                    [this,pSelectStrikes](){
                      auto set = pSelectStrikes->selectedIndexes();
                      using setStrike_t = std::set<std::string>;
                      setStrike_t setStrike;
                      for ( auto& item :set ) {
                        std::string sStrike( pSelectStrikes->itemText( item ).toUTF8() );
                        setStrike.insert( sStrike );
                      }

                      // delete removed entries
                      setStrike_t setDelete;
                      for ( const mapOptionAtStrike_t::value_type& vt: m_mapOptionAtStrike ) {
                        if ( setStrike.end() == setStrike.find( vt.first ) ) {
                          m_pServer->DelStrike( vt.first );
                          setDelete.insert( vt.first );
                        }
                      }
                      for ( const setStrike_t::value_type& vt: setDelete ) {
                        mapOptionAtStrike_t::iterator iter = m_mapOptionAtStrike.find( vt );
                        assert( m_mapOptionAtStrike.end() != iter );
                        iter->second.m_pcw->removeFromParent();
                        m_mapOptionAtStrike.erase( iter );
                      }

                      // insert added entries
                      int ix {};
                      for ( const setStrike_t::value_type& vt: setStrike ) {
                        if ( m_mapOptionAtStrike.end() == m_mapOptionAtStrike.find( vt ) ) {

                          Wt::WContainerWidget* pOptionRow = m_pContainerTableEntry->insertWidget( ix, std::make_unique<Wt::WContainerWidget>() );
                          auto pair = m_mapOptionAtStrike.emplace( vt, OptionAtStrike( pOptionRow ) );
                          assert( pair.second );
                          OptionAtStrike& oas( pair.first->second );

                          Wt::WLabel* pTicker = pOptionRow->addWidget( std::make_unique<Wt::WLabel>( "Ticker" ) );

                          switch ( m_pButtonGroupSide->checkedId() ) {
                            case (int)ESide::buy:
                              pOptionRow->addWidget( std::make_unique<Wt::WLabel>( "Buy" ) );
                              break;
                            case (int)ESide::sell:
                              pOptionRow->addWidget( std::make_unique<Wt::WLabel>( "Sell" ) );
                              break;
                            default:
                              assert( false );
                              break;
                          }

                          pOptionRow->addWidget( std::make_unique<Wt::WLabel>( vt ) ); // strike

                          Server::EOptionType type;
                          switch ( m_pButtonGroupOption->checkedId() ) {
                            case (int)EOption::call:
                              type = Server::EOptionType::call;
                              pOptionRow->addWidget( std::make_unique<Wt::WLabel>( "Call" ) );
                              break;
                            case (int)EOption::put:
                              type = Server::EOptionType::put;
                              pOptionRow->addWidget( std::make_unique<Wt::WLabel>( "Put" ) );
                              break;
                            default:
                              assert( false );
                              break;
                          }

                          Wt::WLabel* pOI  = pOptionRow->addWidget( std::make_unique<Wt::WLabel>( "OpenInt" ) );
                          Wt::WLabel* pBid = pOptionRow->addWidget( std::make_unique<Wt::WLabel>( "Bid" ) );
                          Wt::WLabel* pAsk = pOptionRow->addWidget( std::make_unique<Wt::WLabel>( "Ask" ) );
                          Wt::WLabel* pVol = pOptionRow->addWidget( std::make_unique<Wt::WLabel>( "Volume" ) );
                          Wt::WLineEdit* pWLineEditAlloc = pOptionRow->addWidget( std::make_unique<Wt::WLineEdit>() );
                          Wt::WLabel* pAllocated = pOptionRow->addWidget( std::make_unique<Wt::WLabel>( "Allocated" ) );
                          Wt::WLabel* pNumContracts = pOptionRow->addWidget( std::make_unique<Wt::WLabel>( "#Contracts" ) );
                          Wt::WLineEdit* pEntry = pOptionRow->addWidget( std::make_unique<Wt::WLineEdit>( "mkt|bid|ask|man" ) );
                          Wt::WLineEdit* pScale = pOptionRow->addWidget( std::make_unique<Wt::WLineEdit>( "scale" ) );
                          Wt::WLabel* pPnL = pOptionRow->addWidget( std::make_unique<Wt::WLabel>( "PnL" ) );
                          Wt::WLabel* pFillPrice = pOptionRow->addWidget( std::make_unique<Wt::WLabel>( "FillPrice" ) );

                          m_pServer->AddStrike(
                            type, vt,
                            [pTicker,pOI](const std::string& sTicker, const std::string& sOpenInt ){ // fPopulateOption_t
                              pTicker->setText( sTicker );
                              pOI->setText( sOpenInt );
                            },
                            [pBid,pAsk,pVol,pPnL](const std::string& sBid, const std::string& sAsk, const std::string& sVolume, const std::string& sPnL ) { // fRealTime_t
                              pBid->setText( sBid );
                              pAsk->setText( sAsk );
                              pVol->setText( sVolume );
                              pPnL->setText( sPnL );
                            },
                            [pFillPrice](const std::string& sFill ){ // fFill_t
                              pFillPrice->setText( sFill );
                            }
                            );
                        }
                        ix++;
                      }
                    });
                }
                );
            });
          triggerUpdate();
        }
      );
    } );

  //auto timer = addChild(std::make_unique<Wt::WTimer>());
  //timer->setInterval( std::chrono::seconds( 1 ) );
  //timer->timeout().connect(this, &TimeRecord::HandleTimer );
  //timer->start();

}
