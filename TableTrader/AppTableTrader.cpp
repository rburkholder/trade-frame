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
#include <Wt/WTimer.h>
#include <Wt/WAnchor.h>
#include <Wt/WLineEdit.h>
//#include <Wt/WTextArea.h>
#include <Wt/WGroupBox.h>
#include <Wt/WComboBox.h>
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
  m_pServer->SessionAttach( sessionId() );
  m_timerLiveRefresh = root()->addChild( std::make_unique<Wt::WTimer>() );
  m_timerLiveRefresh->timeout().connect( this, &AppTableTrader::HandleLiveRefresh );
  m_timerLiveRefresh->setInterval (std::chrono::seconds( 1 ) );
  m_timerLiveRefresh->start();
}

void AppTableTrader::finalize() {
  Wt::WApplication::log( "info" ) << "AppTableTrader::finalize()";
  m_timerLiveRefresh->stop();
  m_pServer->SessionDetach( sessionId() );
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

void AppTableTrader::HandleLiveRefresh() {
  m_pServer->TriggerUpdates( sessionId() );
  triggerUpdate();
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
  m_pContainerDataEntry->addStyleClass( "block" );

    Wt::WContainerWidget* pContainerUnderlying = m_pContainerDataEntry->addWidget( std::make_unique<Wt::WContainerWidget>() );
    pContainerUnderlying->addStyleClass( "block" );

      Wt::WLabel* pLabelUnderlying = pContainerUnderlying->addWidget( std::make_unique<Wt::WLabel>( "Select Future: " ) );
      pContainerUnderlying->addWidget( std::make_unique<Wt::WBreak>() );
      Wt::WSelectionBox* pSelectUnderlying = pContainerUnderlying->addWidget( std::make_unique<Wt::WSelectionBox>() );
      pSelectUnderlying->setSelectionMode( Wt::SelectionMode::Single );
      pSelectUnderlying->setVerticalSize( 2 );
      //pLabelUnderlying->setBuddy( pSelectUnderlying );
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

    pLabelUnderlyingLabel->addStyleClass( "w_label" );
    pLabelUnderlyingName->addStyleClass( "w_label" );
    pLabelMultiplierLabel->addStyleClass( "w_label" );
    pLabelMultiplierValue->addStyleClass( "w_label" );

    Wt::WLabel* pLabel = m_pContainerLiveData->addWidget( std::make_unique<Wt::WLabel>( "Current: " ) );
    Wt::WLabel* pLivePrice = m_pContainerLiveData->addWidget( std::make_unique<Wt::WLabel>( "" ) );

    pLabel->addStyleClass( "w_label" );
    pLivePrice->addStyleClass( "w_label" );
    pLivePrice->addStyleClass( "fld_price" );

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
      pContainerExpiries->addStyleClass( "inline" );

        Wt::WContainerWidget* pContainerSpecifications = pContainerExpiries->addWidget( std::make_unique<Wt::WContainerWidget>() );
        pContainerSpecifications->addStyleClass( "block" );

        Wt::WContainerWidget* pContainerSelectExpiries = pContainerExpiries->addWidget( std::make_unique<Wt::WContainerWidget>() );
        pContainerSelectExpiries->addStyleClass( "block" );

          Wt::WLabel* pLabelExpiries = pContainerSelectExpiries->addWidget( std::make_unique<Wt::WLabel>( "Option Chain Expiries: " ) );
          pContainerSelectExpiries->addWidget( std::make_unique<Wt::WBreak>()  );
          Wt::WLabel* pLabelBackgroundLoading = pContainerSelectExpiries->addWidget( std::make_unique<Wt::WLabel>( "Loading (may take a few minutes) ..." ) );
          Wt::WSelectionBox* pSelectExpiries = pContainerSelectExpiries->addWidget( std::make_unique<Wt::WSelectionBox>() );
          pSelectExpiries->setSelectionMode( Wt::SelectionMode::Single );
          pSelectExpiries->setVerticalSize( 4 );
          pSelectExpiries->setHidden( true );
          //pLabelExpiries->setBuddy( pSelectExpiries );


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
        [pLabelBackgroundLoading,pSelectExpiries]( const std::string& sDate ){ // fUpdateOptionExpiries_t
          // TODO: implement timer to indicate duration
          if ( pSelectExpiries->isHidden() ) {
            pLabelBackgroundLoading->setHidden( true );
            pSelectExpiries->setHidden( false );
          }
          pSelectExpiries->addItem( sDate );
        },
        [this,pSelectExpiries](){ // fUpdateOptionExpiriesDone_t
          // TODO: disable once filled
          pSelectExpiries->activated().connect(
            [this,pSelectExpiries](){
              pSelectExpiries->setEnabled( false );
              std::string sDate = pSelectExpiries->valueText().toUTF8();
              m_pContainerDataEntry->clear();

              Wt::WContainerWidget* pContainerTypeAndSide = m_pContainerDataEntry->addWidget( std::make_unique<Wt::WContainerWidget>() );
              pContainerTypeAndSide->addStyleClass( "inline_flex" );

                enum class EOption { call = 1, put = 2 };
                {
                  auto containerOption = pContainerTypeAndSide->addWidget( std::make_unique<Wt::WGroupBox>("Option Type") );
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
                  auto containerSide = pContainerTypeAndSide->addWidget( std::make_unique<Wt::WGroupBox>("Operation") );
                  m_pButtonGroupSide = std::make_shared<Wt::WButtonGroup>();
                  Wt::WRadioButton* btnBuy = containerSide->addWidget( std::make_unique<Wt::WRadioButton>( "Buy" ) );
                  //container->addWidget(std::make_unique<Wt::WBreak>());
                  m_pButtonGroupSide->addButton( btnBuy, (int)ESide::buy );
                  Wt::WRadioButton* btnSell = containerSide->addWidget( std::make_unique<Wt::WRadioButton>( "Sell" ) );
                  //container->addWidget(std::make_unique<Wt::WBreak>());
                  m_pButtonGroupSide->addButton( btnSell, (int)ESide::sell );
                  m_pButtonGroupSide->setCheckedButton( m_pButtonGroupSide->button( (int)ESide::buy ) );
                }

                auto pContainerRange = pContainerTypeAndSide->addWidget( std::make_unique<Wt::WGroupBox>("Range Entry") );
                Wt::WLabel* pLabelStrikeFrom = pContainerRange->addWidget( std::make_unique<Wt::WLabel>( "Strike from:" ) );
                Wt::WLineEdit* pWLineEditStrikeFrom = pContainerRange->addWidget( std::make_unique<Wt::WLineEdit>() );
                Wt::WLabel* pLabelStrikeTo = pContainerRange->addWidget( std::make_unique<Wt::WLabel>( "to" ) );
                Wt::WLineEdit* pWLineEditStrikeTo = pContainerRange->addWidget( std::make_unique<Wt::WLineEdit>() );
                Wt::WLabel* pLabelStrikeInc = pContainerRange->addWidget( std::make_unique<Wt::WLabel>( "inc" ) );
                Wt::WLineEdit* pWLineEditStrikeInc = pContainerRange->addWidget( std::make_unique<Wt::WLineEdit>() );
                Wt::WPushButton* pBtnStrikeAdd = pContainerRange->addWidget( std::make_unique<Wt::WPushButton>( "Add" ) );
                pContainerRange->addWidget( std::make_unique<Wt::WBreak>() );
                Wt::WLabel* pLabelRangeMessage = pContainerRange->addWidget( std::make_unique<Wt::WLabel>() );
                pLabelRangeMessage->setHidden( true );
                pLabelRangeMessage->addStyleClass( "w_label" );
                pLabelRangeMessage->addStyleClass( "fld_message_error" );

                pLabelStrikeFrom->addStyleClass( "w_label" );
                pWLineEditStrikeFrom->addStyleClass( "w_line_edit" );
                pWLineEditStrikeFrom->addStyleClass( "fld_strike" );

                pLabelStrikeTo->addStyleClass( "w_label" );
                pWLineEditStrikeTo->addStyleClass( "w_line_edit" );
                pWLineEditStrikeTo->addStyleClass( "fld_strike" );

                pLabelStrikeInc->addStyleClass( "w_label" );
                pWLineEditStrikeInc->addStyleClass( "w_line_edit" );
                pWLineEditStrikeInc->addStyleClass( "fld_strike" );

                pBtnStrikeAdd->addStyleClass( "w_push_button" );
                // pBtnStrikeAdd.connect is lower down after dependency on pSelectStrikes

              m_pContainerDataEntry->addWidget( std::make_unique<Wt::WBreak>() );

              Wt::WContainerWidget* pContainerAllocationInfo = m_pContainerDataEntry->addWidget( std::make_unique<Wt::WContainerWidget>() );
              pContainerAllocationInfo->addStyleClass( "inline" );

                Wt::WLabel* pLabelAllocated = pContainerAllocationInfo->addWidget( std::make_unique<Wt::WLabel>( "Allocated: " ) );
                pLabelAllocated->addStyleClass( "w_label" );
                Wt::WLabel* pWLabelTotalAllocated = pContainerAllocationInfo->addWidget( std::make_unique<Wt::WLabel>() );
                pWLabelTotalAllocated->addStyleClass( "w_label" );
                pWLabelTotalAllocated->addStyleClass( "fld_allocation" );
                //pLabelAllocated->setBuddy( pWLabelTotalAllocated );
                pWLabelTotalAllocated->setText( "0" );

                Wt::WLabel* pLabelInvestment = pContainerAllocationInfo->addWidget( std::make_unique<Wt::WLabel>( "of" ) );
                pLabelInvestment->addStyleClass( "w_label" );
                Wt::WLineEdit* pWLineEditInvestment = pContainerAllocationInfo->addWidget( std::make_unique<Wt::WLineEdit>() );
                pWLineEditInvestment->addStyleClass( "w_line_edit" );
                pWLineEditInvestment->addStyleClass( "fld_allocation" );
                pLabelInvestment->setBuddy( pWLineEditInvestment );
                pWLineEditInvestment->setText( "100000" );
                m_pServer->ChangeInvestment( pWLineEditInvestment->text().toUTF8() ); // prime the amount
                pWLineEditInvestment->changed().connect(
                  [this,pWLineEditInvestment](){ // TODO: add validator for integer?
                    m_pServer->ChangeInvestment( pWLineEditInvestment->text().toUTF8() );
                  } );

              m_pContainerDataEntry->addWidget( std::make_unique<Wt::WBreak>() );

              Wt::WContainerWidget* pContainerForSelectAndTable = m_pContainerDataEntry->addWidget( std::make_unique<Wt::WContainerWidget>() );
              pContainerForSelectAndTable->addStyleClass( "inline_flex" );
              pContainerForSelectAndTable->setObjectName( "SelectAndTable" );

                Wt::WContainerWidget* pContainerStrikeList = pContainerForSelectAndTable->addWidget( std::make_unique<Wt::WContainerWidget>() );
                pContainerStrikeList->addStyleClass( "block" );

                  //Wt::WLabel* pLabelStrikes = pContainerStrikeList->addWidget( std::make_unique<Wt::WLabel>( "Strikes: " ) );
                  //pContainerStrikeList->addWidget( std::make_unique<Wt::WBreak>() );
                  Wt::WSelectionBox* pSelectStrikes = pContainerStrikeList->addWidget( std::make_unique<Wt::WSelectionBox>() );
                  pSelectStrikes->setSelectionMode( Wt::SelectionMode::Extended );
                  pSelectStrikes->setVerticalSize( 10 );
                  //pLabelStrikes->setBuddy( pSelectStrikes );

              pBtnStrikeAdd->clicked().connect(
                [this,pSelectStrikes,pWLineEditStrikeFrom,pWLineEditStrikeTo,pWLineEditStrikeInc,pLabelRangeMessage](){
                  pLabelRangeMessage->setHidden( true );
                  std::string sFrom = pWLineEditStrikeFrom->text().toUTF8();
                  std::string sTo   = pWLineEditStrikeTo->text().toUTF8();
                  std::string sInc  = pWLineEditStrikeInc->text().toUTF8();
                  if ( sFrom.empty() || sTo.empty() || sInc.empty() ) {
                    pLabelRangeMessage->setHidden( false );
                    pLabelRangeMessage->setText( "all fields required, and greater than 0.0" );
                  }
                  else {
                    std::string sMessage;
                    double dblFrom = Server::FormatDouble( sFrom, sMessage );
                    double dblTo   = Server::FormatDouble( sTo,   sMessage );
                    double dblInc  = Server::FormatDouble( sInc,  sMessage );
                    if ( sMessage.empty() ) {
                      if ( ( 0.0 == dblFrom ) || ( 0.0 == dblTo ) || ( 0.0 == dblInc ) ) {
                        pLabelRangeMessage->setHidden( false );
                        pLabelRangeMessage->setText( "one or more fields convert to 0" );
                      }
                      else {
                        if ( dblFrom >= dblTo ) {
                          pLabelRangeMessage->setHidden( false );
                          pLabelRangeMessage->setText( "'to' needs to be greater than 'from'" );
                        }
                        else {
                          if ( dblInc > ( dblTo - dblFrom ) ) {
                            pLabelRangeMessage->setHidden( false );
                            pLabelRangeMessage->setText( "'inc' needs to be less than 'to' - 'from'" );
                          }
                          else {
                            using setSelection_t = std::set<int>;
                            setSelection_t selection = pSelectStrikes->selectedIndexes();

                            for ( double value = dblFrom; value <= dblTo; value += dblInc ) {
                              std::string sStrike = m_pServer->FormatStrike( value );
                              int ix = pSelectStrikes->findText( sStrike );
                              if ( 0 < ix ) { // not sure what an illegal value is
                                if ( 20 <= selection.size() ) {
                                  pLabelRangeMessage->setHidden( false );
                                  pLabelRangeMessage->setText( "attempting insert of >20 strikes" );
                                  break;
                                }
                                else {
                                  selection.insert( ix );
                                }
                              }
                            }

                            pSelectStrikes->setSelectedIndexes( selection );
                            m_fUpdateStrikeSelection();

                          }
                        }
                      }
                    }
                    else {
                      pLabelRangeMessage->setHidden( false );
                      pLabelRangeMessage->setText( sMessage );
                    }
                  }
                });

              Wt::WContainerWidget* pContainerTheTable = pContainerForSelectAndTable->addWidget( std::make_unique<Wt::WContainerWidget>() );
              pContainerStrikeList->addStyleClass( "block" );

              m_pServer->PrepareStrikeSelection(
                sDate,
                [pSelectStrikes](const std::string& sStrike){ // fPopulateStrike_t
                  pSelectStrikes->addItem( sStrike );
                },
                [this,pSelectStrikes,pContainerTheTable,pWLabelTotalAllocated](){ // fPopulateStrikeDone_t
                  Wt::WPushButton* pBtnCancelAll = m_pContainerTableEntryButtons->addWidget( std::make_unique<Wt::WPushButton>( "Cancel All" ) );
                  pBtnCancelAll->setEnabled( false );
                  pBtnCancelAll->addStyleClass( "w_push_button" );
                  Wt::WPushButton* pBtnCloseAll = m_pContainerTableEntryButtons->addWidget( std::make_unique<Wt::WPushButton>( "Cancel/Close All" ) );
                  pBtnCloseAll->setEnabled( false );
                  pBtnCloseAll->addStyleClass( "w_push_button" );
                  Wt::WPushButton* pBtnPlaceOrders = m_pContainerTableEntryButtons->addWidget( std::make_unique<Wt::WPushButton>( "Place Orders" ) );
                  pBtnPlaceOrders->setEnabled( false );

                  m_fUpdateStrikeSelection = // used by pSelectStrikes & pBtnEditAllocDelete
                    [this,pSelectStrikes,pContainerTheTable,pBtnPlaceOrders,pWLabelTotalAllocated](){
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

                          Wt::WContainerWidget* pOptionRow = pContainerTheTable->insertWidget( ix, std::make_unique<Wt::WContainerWidget>() );
                          auto pair = m_mapOptionAtStrike.emplace( vt, OptionAtStrike( pOptionRow ) );
                          assert( pair.second );
                          OptionAtStrike& oas( pair.first->second );

                          Wt::WLabel* pTicker = pOptionRow->addWidget( std::make_unique<Wt::WLabel>( "Ticker" ) );
                          pTicker->addStyleClass( "w_label" );
                          pTicker->addStyleClass( "fld_ticker" );

                          Wt::WLabel* pSide;
                          Server::EOrderSide side;
                          switch ( m_pButtonGroupSide->checkedId() ) {
                            case (int)ESide::buy:
                              side = Server::EOrderSide::buy;
                              pSide = pOptionRow->addWidget( std::make_unique<Wt::WLabel>( "Buy" ) );
                              break;
                            case (int)ESide::sell:
                              side = Server::EOrderSide::sell;
                              pSide = pOptionRow->addWidget( std::make_unique<Wt::WLabel>( "Sell" ) );
                              break;
                            default:
                              assert( false );
                              break;
                          }
                          pSide->addStyleClass( "w_label" );
                          pSide->addStyleClass( "fld_side" );

                          Wt::WLabel* pStrike = pOptionRow->addWidget( std::make_unique<Wt::WLabel>( vt ) );
                          pStrike->addStyleClass( "w_label" );
                          pStrike->addStyleClass( "fld_strike" );

                          Wt::WLabel* pType;
                          Server::EOptionType type;
                          switch ( m_pButtonGroupOption->checkedId() ) {
                            case (int)EOption::call:
                              type = Server::EOptionType::call;
                              pType = pOptionRow->addWidget( std::make_unique<Wt::WLabel>( "Call" ) );
                              break;
                            case (int)EOption::put:
                              type = Server::EOptionType::put;
                              pType = pOptionRow->addWidget( std::make_unique<Wt::WLabel>( "Put" ) );
                              break;
                            default:
                              assert( false );
                              break;
                          }
                          pType->addStyleClass( "w_label" );
                          pType->addStyleClass( "fld_type" );

                          Wt::WLabel* pOI  = pOptionRow->addWidget( std::make_unique<Wt::WLabel>( "OI" ) );
                          Wt::WLabel* pVol = pOptionRow->addWidget( std::make_unique<Wt::WLabel>( "Vol" ) );
                          Wt::WLabel* pBid = pOptionRow->addWidget( std::make_unique<Wt::WLabel>( "Bid" ) );
                          Wt::WLabel* pAsk = pOptionRow->addWidget( std::make_unique<Wt::WLabel>( "Ask" ) );
                          Wt::WLineEdit* pWLineEditAlloc = pOptionRow->addWidget( std::make_unique<Wt::WLineEdit>() );
                          Wt::WPushButton* pBtnEditAllocDelete= pOptionRow->addWidget( std::make_unique<Wt::WPushButton>( "X" ) );
                          Wt::WLabel* pAllocated = pOptionRow->addWidget( std::make_unique<Wt::WLabel>( "0" ) );
                          Wt::WLabel* pNumContracts = pOptionRow->addWidget( std::make_unique<Wt::WLabel>( "0" ) );
                          Wt::WComboBox* pOrderType = pOptionRow->addWidget( std::make_unique<Wt::WComboBox>() );
                          Wt::WLineEdit* pLimitPrice = pOptionRow->addWidget( std::make_unique<Wt::WLineEdit>( "0" ) ); // limit, scale
                          Wt::WLineEdit* pScaleInitialQuan = pOptionRow->addWidget( std::make_unique<Wt::WLineEdit>( "0" ) ); // scale
                          Wt::WLineEdit* pScaleIncQuan = pOptionRow->addWidget( std::make_unique<Wt::WLineEdit>( "0" ) ); // scale
                          Wt::WLineEdit* pScaleIncPrice = pOptionRow->addWidget( std::make_unique<Wt::WLineEdit>( "0" ) ); // scale
                          Wt::WLabel* pPnL = pOptionRow->addWidget( std::make_unique<Wt::WLabel>( "-" ) );
                          Wt::WLabel* pEntryFillPrice = pOptionRow->addWidget( std::make_unique<Wt::WLabel>( "-" ) );
                          Wt::WLabel* pExitFillPrice = pOptionRow->addWidget( std::make_unique<Wt::WLabel>( "-" ) );
                          pOptionRow->addWidget( std::make_unique<Wt::WBreak>() );
                          Wt::WLabel* pMessage = pOptionRow->addWidget( std::make_unique<Wt::WLabel>( "" ) );
                          pMessage->setHidden( true );

                          pOI->addStyleClass( "w_label" );
                          pOI->addStyleClass( "fld_open_interest" );

                          pVol->addStyleClass( "w_label" );
                          pVol->addStyleClass( "fld_volume" );

                          pBid->addStyleClass( "w_label" );
                          pBid->addStyleClass( "fld_quote" );

                          pAsk->addStyleClass( "w_label" );
                          pAsk->addStyleClass( "fld_quote" );

                          pBtnEditAllocDelete->addStyleClass( "w_push_button_x" );
                          pBtnEditAllocDelete->clicked().connect(
                            [this,vt,pSelectStrikes](){
                              int ixThisEntry = pSelectStrikes->findText( vt );
                              m_pServer->post( // can't delete self while in code
                                sessionId(),
                                [this,pSelectStrikes,ixThisEntry](){
                                  using setSelection_t = std::set<int>;
                                  setSelection_t selection = pSelectStrikes->selectedIndexes();
                                  setSelection_t::iterator iter = selection.find( ixThisEntry );
                                  if ( selection.end() != iter ) {
                                    selection.erase( iter );
                                    pSelectStrikes->setSelectedIndexes( selection );
                                    m_fUpdateStrikeSelection();
                                  }
                                });
                            });

                          pWLineEditAlloc->setText( "0" );
                          //pWLineEditAlloc->keyWentUp().connect( [](){} );
                          pWLineEditAlloc->changed().connect(
                            [this,vt,pWLineEditAlloc](){
                              m_pServer->ChangeAllocation( vt, pWLineEditAlloc->text().toUTF8() );
                            } );
                          pWLineEditAlloc->addStyleClass( "w_line_edit" );
                          pWLineEditAlloc->addStyleClass( "fld_percent_allocation" );

                          pAllocated->addStyleClass( "w_label" );
                          pAllocated->addStyleClass( "fld_allocation" );

                          pNumContracts->addStyleClass( "w_label" );
                          pNumContracts->addStyleClass( "fld_num_contracts" );

                          pOrderType->addItem( "market" );
                          pOrderType->addItem( "limit" );
                          pOrderType->addItem( "lim ask" );
                          pOrderType->addItem( "lim bid" );
                          pOrderType->addItem( "scale" );
                          pOrderType->addStyleClass( "w_combo_box" );

                          pOrderType->activated().connect(
                            [this,vt,pOrderType,pLimitPrice,pScaleInitialQuan,pScaleIncQuan,pScaleIncPrice,pBid,pAsk,pMessage](){
                              std::string sMessage = ComposeOrderType(
                                pOrderType->currentIndex(),
                                vt,
                                pAsk,pBid,pLimitPrice, pScaleInitialQuan, pScaleIncQuan, pScaleIncPrice
                              );
                              pMessage->setText( sMessage );
                              if ( sMessage.empty() ) pMessage->setHidden( true );
                              else {
                                pMessage->setHidden( false );
                              }
                            } );

                          pLimitPrice->setEnabled( false );
                          pLimitPrice->addStyleClass( "w_line_edit" );
                          pLimitPrice->addStyleClass( "fld_price" );
                          pLimitPrice->changed().connect(
                            [this,vt,pOrderType,pLimitPrice,pScaleInitialQuan,pScaleIncQuan,pScaleIncPrice,pBid,pAsk,pMessage](){
                              std::string sMessage = ComposeOrderType(
                                pOrderType->currentIndex(),
                                vt,
                                pAsk,pBid,pLimitPrice, pScaleInitialQuan, pScaleIncQuan, pScaleIncPrice
                              );
                              pMessage->setText( sMessage );
                              if ( sMessage.empty() ) pMessage->setHidden( true );
                              else {
                                pMessage->setHidden( false );
                              }
                            } );

                          pScaleInitialQuan->setEnabled( false );
                          pScaleInitialQuan->addStyleClass( "w_line_edit" );
                          pScaleInitialQuan->addStyleClass( "fld_num_contracts" );
                          pScaleInitialQuan->changed().connect(
                            [this,vt,pOrderType,pLimitPrice,pScaleInitialQuan,pScaleIncQuan,pScaleIncPrice,pBid,pAsk,pMessage](){
                              std::string sMessage = ComposeOrderType(
                                pOrderType->currentIndex(),
                                vt,
                                pAsk,pBid,pLimitPrice, pScaleInitialQuan, pScaleIncQuan, pScaleIncPrice
                              );
                              pMessage->setText( sMessage );
                              if ( sMessage.empty() ) pMessage->setHidden( true );
                              else {
                                pMessage->setHidden( false );
                              }
                            } );

                          pScaleIncQuan->setEnabled( false );
                          pScaleIncQuan->addStyleClass( "w_line_edit" );
                          pScaleIncQuan->addStyleClass( "fld_num_contracts" );
                          pScaleIncQuan->changed().connect(
                            [this,vt,pOrderType,pLimitPrice,pScaleInitialQuan,pScaleIncQuan,pScaleIncPrice,pBid,pAsk,pMessage](){
                              std::string sMessage = ComposeOrderType(
                                pOrderType->currentIndex(),
                                vt,
                                pAsk,pBid,pLimitPrice, pScaleInitialQuan, pScaleIncQuan, pScaleIncPrice
                              );
                              pMessage->setText( sMessage );
                              if ( sMessage.empty() ) pMessage->setHidden( true );
                              else {
                                pMessage->setHidden( false );
                              }
                            } );

                          pScaleIncPrice->setEnabled( false );
                          pScaleIncPrice->addStyleClass( "w_line_edit" );
                          pScaleIncPrice->addStyleClass( "fld_price" );
                          pScaleIncPrice->changed().connect(
                            [this,vt,pOrderType,pLimitPrice,pScaleInitialQuan,pScaleIncQuan,pScaleIncPrice,pBid,pAsk,pMessage](){
                              std::string sMessage = ComposeOrderType(
                                pOrderType->currentIndex(),
                                vt,
                                pAsk,pBid,pLimitPrice, pScaleInitialQuan, pScaleIncQuan, pScaleIncPrice
                              );
                              pMessage->setText( sMessage );
                              if ( sMessage.empty() ) pMessage->setHidden( true );
                              else {
                                pMessage->setHidden( false );
                              }
                            } );

                          pPnL->addStyleClass( "w_label" );
                          pPnL->addStyleClass( "fld_pnl" );

                          pEntryFillPrice->addStyleClass( "w_label" );
                          pEntryFillPrice->addStyleClass( "fld_price" );

                          pExitFillPrice->addStyleClass( "w_label" );
                          pExitFillPrice->addStyleClass( "fld_price" );

                          pMessage->addStyleClass( "w_label" );
                          pMessage->addStyleClass( "fld_message_error" );

                          m_pServer->AddStrike(
                            sessionId(),
                            type, side, vt,
                            [pTicker](const std::string& sTicker){ // fPopulateOption_t
                              pTicker->setText( sTicker );
                            },
                            [pWLabelTotalAllocated,pAllocated](const std::string& sTotalAllocated, const std::string& sOptionAllocated ){ // fUpdateAllocated_t
                              pWLabelTotalAllocated->setText( sTotalAllocated );
                              pAllocated->setText( sOptionAllocated );
                            },
                            [pBid,pAsk,pVol,pNumContracts,pPnL,pOI](const std::string& sOpenInt, const std::string& sBid, const std::string& sAsk, const std::string& sVolume, const std::string& sContracts, const std::string& sPnL ) { // fRealTime_t
                              pOI->setText( sOpenInt );
                              pBid->setText( sBid );
                              pAsk->setText( sAsk );
                              pVol->setText( sVolume );
                              pNumContracts->setText( sContracts );
                              pPnL->setText( sPnL );
                            },
                            [this,pEntryFillPrice](const std::string& sFill ){ // fFill_t async
                              if ( pEntryFillPrice->text().empty() ) {
                                pEntryFillPrice->setText( sFill );
                              }
                              else {
                                pEntryFillPrice->text() += "," + sFill;
                              }
                              triggerUpdate();
                            },
                            [this,pExitFillPrice](const std::string& sFill ){ // fFill_t async
                              pExitFillPrice->setText( sFill );
                              triggerUpdate();
                            }
                            );
                        }
                        ix++;
                      } // end: add entries

                      pBtnPlaceOrders->setEnabled( 0 < m_mapOptionAtStrike.size() );
                    }; // pSelectStrikes->changed()

                    pSelectStrikes->changed().connect( // only this one works with multiple selection
                      [this](){
                        m_fUpdateStrikeSelection();
                      });

                    pBtnCancelAll->clicked().connect(
                      [this,pBtnCancelAll](){
                        pBtnCancelAll->setEnabled( false );
                        m_pServer->CancelAll();
                      });
                    pBtnCloseAll->clicked().connect(
                      [this,pBtnCancelAll,pBtnCloseAll](){
                        pBtnCancelAll->setEnabled( false );
                        m_pServer->CancelAll();
                        pBtnCloseAll->setEnabled( false );
                        m_pServer->CloseAll();
                      });
                    pBtnPlaceOrders->clicked().connect(
                      [this,pSelectStrikes,pBtnCancelAll,pBtnCloseAll,pBtnPlaceOrders](){
                        if ( m_pServer->PlaceOrders() ) {
                          pSelectStrikes->setEnabled( false );
                          pBtnCancelAll->setEnabled( true );
                          pBtnCloseAll->setEnabled( true );
                          pBtnPlaceOrders->setEnabled (false );
                        }
                      });
                }
                ); // m_pServer->PrepareStrikeSelection
            });
          triggerUpdate();
        }
      ); // m_pServer->Start
    } ); // pSelectUnderlying->activated()
}

std::string AppTableTrader::ComposeOrderType(
  int ixOrderType,
  const std::string& strike,
  Wt::WLabel* pAsk,
  Wt::WLabel* pBid,
  Wt::WLineEdit* pLimitPrice,
  Wt::WLineEdit* pInitialQuantity,
  Wt::WLineEdit* pIncrementQuantity,
  Wt::WLineEdit* pIncrementPrice
) {

  pLimitPrice->setEnabled( false );
  pInitialQuantity->setEnabled( false );
  pIncrementQuantity->setEnabled( false );
  pIncrementPrice->setEnabled( false );

  Server::EOrderType eOrderType;

  switch ( ixOrderType ) {
    case (int) Server::EOrderType::market:
      eOrderType = Server::EOrderType::market;
      break;
    case (int) Server::EOrderType::limit_manual:
      eOrderType = Server::EOrderType::limit_manual;
      pLimitPrice->setEnabled( true );
      break;
    case (int) Server::EOrderType::limit_ask:
      eOrderType = Server::EOrderType::limit_ask;
      pLimitPrice->setText( pAsk->text() );
      pLimitPrice->setEnabled( true );
      break;
    case (int) Server::EOrderType::limit_bid:
      eOrderType = Server::EOrderType::limit_bid;
      pLimitPrice->setText( pBid->text() );
      pLimitPrice->setEnabled( true );
      break;
    case (int) Server::EOrderType::scale:
      eOrderType = Server::EOrderType::scale;
      pLimitPrice->setEnabled( true );
      pInitialQuantity->setEnabled( true );
      pIncrementQuantity->setEnabled( true );
      pIncrementPrice->setEnabled( true );
      break;
  }

  // TODO: use validators on the original ui fields
  std::string sMessage = m_pServer->SetOrderType(
    eOrderType, strike,
    pLimitPrice->text().toUTF8(),
    pInitialQuantity->text().toUTF8(),
    pIncrementQuantity->text().toUTF8(),
    pIncrementPrice->text().toUTF8()
  );

  return sMessage;
}
