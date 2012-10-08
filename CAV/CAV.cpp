/************************************************************************
 * Copyright(c) 2012, One Unified. All rights reserved.                 *
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

// CAV.cpp : Defines the entry point for the application.
//

#include "stdafx.h"

#include "CAV.h"
IMPLEMENT_APP(AppCollectAndView)

#include <fstream>
#include <vector>
#include <map>

#include <OUCommon/KeywordMatch.h>

#include <TFTrading/InstrumentManager.h>
#include <TFTrading/AccountManager.h>
#include <TFTrading/OrderManager.h>
#include <TFTrading/PortfolioManager.h>

#include <TFIQFeed/ParseMktSymbols.h>
#include <TFIQFeed/ParseOptionDescription.h>

bool AppCollectAndView::OnInit() {

  m_pFrameMain = new FrameMain( 0, wxID_ANY, "Collect And View" );
  wxWindowID idFrameMain = m_pFrameMain->GetId();
  //m_pFrameMain->Bind( wxEVT_SIZE, &AppStrategy1::HandleFrameMainSize, this, idFrameMain );
  //m_pFrameMain->Bind( wxEVT_MOVE, &AppStrategy1::HandleFrameMainMove, this, idFrameMain );
  //m_pFrameMain->Center();
//  m_pFrameMain->Move( -2500, 50 );
  m_pFrameMain->SetSize( 500, 600 );
  SetTopWindow( m_pFrameMain );

  wxBoxSizer* m_sizerMain;
  m_sizerMain = new wxBoxSizer(wxVERTICAL);
  m_pFrameMain->SetSizer(m_sizerMain);

  wxBoxSizer* m_sizerControls;
  m_sizerControls = new wxBoxSizer( wxHORIZONTAL );
  m_sizerMain->Add( m_sizerControls, 0, wxLEFT|wxTOP|wxRIGHT, 5 );

  m_pPanelProviderControl = new ou::tf::PanelProviderControl( m_pFrameMain, wxID_ANY );
  m_sizerControls->Add( m_pPanelProviderControl, 1, wxEXPAND|wxALIGN_LEFT|wxRIGHT, 5);
  m_pPanelProviderControl->Show( true );

  LinkToPanelProviderControl();
/*
  m_pPanelOptionsParameters = new PanelOptionsParameters( m_pFrameMain, wxID_ANY );
  m_sizerControls->Add( m_pPanelOptionsParameters, 1, wxEXPAND|wxALIGN_LEFT, 0);
  m_pPanelOptionsParameters->Show( true );
  m_pPanelOptionsParameters->SetOnStart( MakeDelegate( this, &AppStrategyRunner::HandleBtnStart ) );
  m_pPanelOptionsParameters->SetOnStop( MakeDelegate( this, &AppStrategyRunner::HandleBtnStop ) );
  m_pPanelOptionsParameters->SetOnSave( MakeDelegate( this, &AppStrategyRunner::HandleBtnSave ) );
  m_pPanelOptionsParameters->SetOptionNearDate( boost::gregorian::date( 2012, 4, 20 ) );
  m_pPanelOptionsParameters->SetOptionFarDate( boost::gregorian::date( 2012, 6, 15 ) );
*/
  wxBoxSizer* m_sizerStatus = new wxBoxSizer( wxHORIZONTAL );
  m_sizerMain->Add( m_sizerStatus, 1, wxEXPAND|wxALL, 5 );

  m_pPanelLogging = new ou::tf::PanelLogging( m_pFrameMain, wxID_ANY );
  m_sizerStatus->Add( m_pPanelLogging, 1, wxALL | wxEXPAND|wxALIGN_LEFT|wxALIGN_RIGHT|wxALIGN_TOP|wxALIGN_BOTTOM, 0);
  m_pPanelLogging->Show( true );

  m_pFrameMain->Show( true );

  // load mktsymbols somewhere

    std::ifstream file;
    char* name = "mktsymbols_v2.txt";
    std::cout << "Opening Input Instrument File ";
    std::cout << name;
    std::cout << " ... ";
    file.open( name );
    std::cout << std::endl;

    std::cout << "Loading Symbols ..." << std::endl;

    char line[ 500 ];

    typedef ou::tf::iqfeed::MarketSymbol::enumSymbolClassifier sc_t;
    ou::tf::iqfeed::MktSymbolsParser<const char*> parser;
    std::vector<size_t> vSymbolTypeStats( sc_t::_Count );  // number of symbols of this SymbolType

    ou::tf::iqfeed::OptionDescriptionParser<std::string::const_iterator> parserOption;

    struct structCountPerString { // count cnt of string s
      size_t cnt;
      std::string s;
      structCountPerString( void ) : cnt( 0 ) {};
      bool operator<(const structCountPerString& rhs) { return (s < rhs.s); };
    };

    ou::KeyWordMatch<size_t> kwmExchanges( 0, 200 );  // about 300 characters?  ... fast look up of index into m_rExchanges, possibly faster than std::map
    kwmExchanges.AddPattern( "Unknown", 0 );
    std::vector<structCountPerString> vSymbolsPerExchange( 1 );
    vSymbolsPerExchange[ 0 ].s = "UNKNOWN";

    unsigned short nUnderlyingSize = 0;
    std::map<std::string, unsigned long> mapUnderlying;  // keeps track of optionable symbols, to fix bool at end

    size_t cntLinesTotal = 0;
    size_t cntLinesParsed = 0;
    size_t cntSIC = 0;
    size_t cntNAICS = 0;

    file.getline( line, 500 );  // remove header line
    file.getline( line, 500 );
    while ( !file.fail() ) {

      ++cntLinesTotal;  // number data lines processed

      ou::tf::iqfeed::MarketSymbol::TableRowDef trd;
      const char* pLine1( line );
      const char* pLine2( line + 500 );

      // try http://stackoverflow.com/questions/2291802/is-there-a-c-iterator-that-can-iterate-over-a-file-line-by-line

      try {
        bool b = qi::parse( pLine1, pLine2, parser, trd );
        if ( !b ) {
          std::cout << "problems parsing" << std::endl;
        }
        else {

          cntLinesParsed++;

          size_t ix;

          vSymbolTypeStats[ trd.sc ]++;
          if ( sc_t::Unknown == trd.sc ) {
            // set marker not to save record?
            std::cout << "Unknown symbol type for:  " << trd.sSymbol << std::endl;
          }

          std::string sPattern( trd.sExchange );
          if ( trd.sExchange == trd.sListedMarket ) {
          }
          else {
            sPattern += "," + trd.sListedMarket;
          }

          if ( 0 == sPattern.length() ) {
            std::cout << trd.sSymbol << " has zero length exchange,market" << std::endl;
          }
          else {
            ix = kwmExchanges.FindMatch( sPattern );
            if ( ( 0 == ix ) || ( sPattern.length() != vSymbolsPerExchange[ ix ].s.length() ) ) {
              std::cout << "Adding Exchange " << sPattern << std::endl;
              size_t cnt = kwmExchanges.GetPatternCount();
              kwmExchanges.AddPattern( sPattern, cnt );
              structCountPerString cps;
              vSymbolsPerExchange.push_back( cps );
              vSymbolsPerExchange[ cnt ].cnt = 1;
              vSymbolsPerExchange[ cnt ].s = sPattern;
            }
            else {
              ++( vSymbolsPerExchange[ ix ].cnt );
            }
          }

          if ( ou::tf::iqfeed::MarketSymbol::Equity == trd.sc ) {
            if ( 0 != trd.sSIC.length() ) cntSIC++;
            if ( 0 != trd.sNAICS.length() ) cntNAICS++;
          }


          // parse out contract expiry information
          // � For combined session symbols, the first character is "+".
          //� For Night/Electronic sessions, the first character is "@".
          // � Replace the Month and Year code with "#" for Front Month (ie. @ES# instead of @ESU10).
          // � NEW!-Replace the Month and Year code with "#C" for Front Month back-adjusted history (ie. @ES#C instead of @ESU10). 
          // http://www.iqfeed.net/symbolguide/index.cfm?symbolguide=guide&displayaction=support&section=guide&web=iqfeed&guide=commod&web=IQFeed&symbolguide=guide&displayaction=support&section=guide&type=comex&type2=comex_gbx
          if ( ou::tf::iqfeed::MarketSymbol::Future == trd.sc ) {
            bool bDecode = true;
  //          if ( '+' == sSymbol[0] ) {
  //          }
  //          if ( '@' == sSymbol[0] ) {
  //          }
            if ( '#' == trd.sSymbol[ trd.sSymbol.length() - 1 ] ) {
              bDecode = false;
            }
            if ( bDecode ) {
              std::string sYear = trd.sSymbol.substr( trd.sSymbol.length() - 2 );
              char mon = trd.sSymbol[ trd.sSymbol.length() - 3 ];
              if ( ( 'F' > mon ) || ( 'Z' < mon ) || ( 0 == ou::tf::iqfeed::rFutureMonth[ mon - 'A' ] ) ) {
                std::cout << "Bad future month on " << trd.sSymbol << ": " << trd.sDescription << std::endl;
              }
              else {
                trd.nMonth = ou::tf::iqfeed::rFutureMonth[ mon - 'A' ];
                trd.nYear = 2000 + atoi( sYear.c_str() );
              }
            }
          }

          // parse out option contract information
          if ( ou::tf::iqfeed::MarketSymbol::IEOption == trd.sc ) {
            ou::tf::iqfeed::structParsedOptionDescription structOption( trd.nMonth, trd.nYear, trd.eOptionSide, trd.dblStrike );
            std::string::const_iterator sb( trd.sDescription.begin() );
            std::string::const_iterator se( trd.sDescription.end() );
            bool b = parse( sb, se, parserOption, structOption );
            if ( b && ( sb == se ) ) {
              if ( 0 == structOption.sUnderlying ) {
                std::cout << "Option Decode:  Zero length underlying for " << trd.sSymbol << std::endl;
              }
              else {
                mapUnderlying[ structOption.sUnderlying ] = 1;  // simply create an entry for later use
              }
              nUnderlyingSize = std::max<unsigned short>( nUnderlyingSize, structOption.sUnderlying.size() );
            }
            else {
              std::cout  << "Option Decode:  Incomplete, " << trd.sSymbol << ", " << trd.sDescription << std::endl;
            }
          }

        }
        if ( 0 == trd.sDescription.length() ) {
          std::cout << trd.sSymbol << ": missing description" << std::endl;
        }
      }
      catch (...) {
        std::cout << "broken" << std::endl;
      }

//      std::cout << trd.sSymbol << std::endl;

      file.getline( line, 500 );
    }
    file.close();

    struct processSymbols {
      void operator()( const std::string& s, ou::tf::iqfeed::MarketSymbol::enumSymbolClassifier sc ) {
        std::cout << s << "=" << v[ sc ] << std::endl;
      }
      processSymbols( std::vector<size_t>& v_ ): v(v_) {
      }
      std::vector<size_t>& v;
    };

    std::cout << std::endl;
//    for ( size_t ix = 0; ix < ou::tf::iqfeed::MarketSymbol::enumSymbolClassifier_count; ++ix ) {
//      std::cout << rSymbolTypes[ ix ].sSC << "=" << vSymbolTypeStats[ ix ] << std::endl;
//    }
    parser.symTypes.for_each( processSymbols( vSymbolTypeStats ) );
    std::cout << std::endl;

    std::cout << "Count Optionables  =" << mapUnderlying.size() << std::endl;
    std::cout << "Max Underlying Size=" << nUnderlyingSize << std::endl;
    std::cout << "cntSIC             =" << cntSIC << std::endl;
    std::cout << "cntNAICS           =" << cntNAICS << std::endl;
    std::cout << std::endl;

    std::sort( vSymbolsPerExchange.begin(), vSymbolsPerExchange.end() );
    for ( size_t ix = 0; ix < vSymbolsPerExchange.size(); ++ix ) {
      std::cout << vSymbolsPerExchange[ ix ].s << "=" << vSymbolsPerExchange[ ix ].cnt << std::endl;
    }
    std::cout << std::endl;

    std::cout << "Symbol List Complete" << std::endl;

#ifdef _DEBUG
  DEBUGOUT( 
    "#kwmExchanges nodes " << kwmExchanges.GetNodeCount() << std::endl 
    )
#endif

//  m_db.SetOnPopulateDatabaseHandler( MakeDelegate( this, &AppCollectAndView::HandlePopulateDatabase ) );
//  m_db.Open( "cav.db" );

  return 1;

}

int AppCollectAndView::OnExit() {

//  DelinkFromPanelProviderControl();  generates stack errors

  if ( m_db.IsOpen() ) m_db.Close();

  return 0;
}

void AppCollectAndView::HandlePopulateDatabase( void ) {

  ou::tf::CAccountManager::pAccountAdvisor_t pAccountAdvisor 
    = ou::tf::CAccountManager::Instance().ConstructAccountAdvisor( "aaRay", "Raymond Burkholder", "One Unified" );

  ou::tf::CAccountManager::pAccountOwner_t pAccountOwner
    = ou::tf::CAccountManager::Instance().ConstructAccountOwner( "aoRay", "aaRay", "Raymond", "Burkholder" );

  ou::tf::CAccountManager::pAccount_t pAccountIB
    = ou::tf::CAccountManager::Instance().ConstructAccount( "ib01", "aoRay", "Raymond Burkholder", ou::tf::keytypes::EProviderIB, "Interactive Brokers", "acctid", "login", "password" );

  ou::tf::CAccountManager::pAccount_t pAccountIQFeed
    = ou::tf::CAccountManager::Instance().ConstructAccount( "iq01", "aoRay", "Raymond Burkholder", ou::tf::keytypes::EProviderIQF, "IQFeed", "acctid", "login", "password" );

  ou::tf::CAccountManager::pAccount_t pAccountSimulator
    = ou::tf::CAccountManager::Instance().ConstructAccount( "sim01", "aoRay", "Raymond Burkholder", ou::tf::keytypes::EProviderSimulator, "Sim", "acctid", "login", "password" );

//  ou::tf::CPortfolioManager::pPortfolio_t pPortfolio
//    //= ou::tf::CPortfolioManager::Instance().ConstructPortfolio( m_idPortfolio, "aoRay", "SemiAuto" );
//    = ou::tf::CPortfolioManager::Instance().ConstructPortfolio( "pflioOptions", "aoRay", "options" );

}

