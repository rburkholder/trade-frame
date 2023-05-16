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
 * File:    Main.cpp
 * Author:  raymond@burkholder.net
 * Project: rdaf/dl
 * Created: May 29, 2022  14:42
 */

#include <mutex>
#include <memory>
#include <future>

#include <boost/asio/post.hpp>
#include <boost/asio/execution/context.hpp>
#include <boost/asio/executor_work_guard.hpp>

#include <boost/filesystem.hpp>

#include <boost/log/trivial.hpp>

#include <rdaf/TRint.h>
#include <rdaf/TROOT.h>
#include <rdaf/TFile.h>
#include <rdaf/TTree.h>

#include <TFIQFeed/Provider.h>
#include <TFIQFeed/SymbolLookup.h>
#include <TFIQFeed/HistoryQuery.h>

#include <TFTrading/AcquireFundamentals.h>

#include "Config.hpp"

using setNames_t = ou::tf::iqfeed::SymbolLookup::setNames_t;

namespace {

  static const size_t nMaxInTransit = 40;

  static const std::string sDirectory( "rdaf/dl" );

  setNames_t setListedMarket;
  setNames_t setSecurityTypes;

  std::unique_ptr<TRint> m_prdafApp;
  std::shared_ptr<TFile> m_pFile; // primary timeseries

  using pTTree_t = std::shared_ptr<TTree>;

  pTTree_t m_pTreeStatistics;

  size_t nErrors {};

}

struct Security {
  std::string sName;
  std::string sListedMarket;
  double dblPriceEarnings;
  size_t nAverageVolume;
  double dblAssets;
  double dblLiabilities;
  double dblCommonSharesOutstanding;
  size_t nSIC;
  size_t nNAICS;

  size_t nTicks; // local counter

  struct QuoteForBranch {
    double time;
    double ask;
    uint64_t askvol;
    double bid;
    uint64_t bidvol;
  } m_branchQuote;

  struct TradeForBranch {
    double time;
    double price;
    uint64_t vol;
    int64_t direction;
  } m_branchTrade;

  // https://root.cern/doc/master/classTTree.html
  pTTree_t m_pTreeQuote;
  TBranch* pBranchQuote;
  TBasket* pBasketQuote;

  pTTree_t m_pTreeTrade;
  TBranch* pBranchTrade;
  TBasket* pBasketTrade;

  Security( const std::string& sName_, const std::string& sListedMarket_ )
  : sName( sName_ ), sListedMarket( sListedMarket_ )
  , dblPriceEarnings {}, nAverageVolume {}
  , dblAssets {}, dblLiabilities {}
  , dblCommonSharesOutstanding {}
  , nTicks {}
  , nSIC {}, nNAICS {}
  {
  }

  ~Security() { // manually delete anything?  branches? baskets? or is that automatically performed via the shared_ptr
  }

  void RdafInit() {

    // https://root.cern.ch/doc/master/classTTree.html

    m_pTreeQuote = std::make_shared<TTree>(
      ( sName + "_quotes" ).c_str(), ( sName + " quotes" ).c_str(), 99, m_pFile.get()
    );
    if ( !m_pTreeQuote ) {
      BOOST_LOG_TRIVIAL(error) << "problems m_pTreeQuote";
    }
    else {
      pBranchQuote
        = m_pTreeQuote->Branch( "quote", &m_branchQuote, "time/D:ask/D:askvol/l:bid/D:bidvol/l" );
      pBasketQuote = m_pTreeQuote->CreateBasket( pBranchQuote );
    }

    m_pTreeTrade = std::make_shared<TTree>(
      ( sName + "_trades" ).c_str(), ( sName + " trades" ).c_str(), 99, m_pFile.get()
    );
    if ( !m_pTreeTrade ) {
      BOOST_LOG_TRIVIAL(error) << "problems m_pTreeTrade";
    }
    else {
      pBranchTrade
        = m_pTreeTrade->Branch( "trade", &m_branchTrade, "time/D:price/D:vol/l:direction/L" );
      pBasketTrade = m_pTreeTrade->CreateBasket( pBranchTrade );
    }
  }

  void RdafDirectory() { // called from thread in which pFile was created
    m_pTreeQuote->SetDirectory( m_pFile.get() );
    m_pTreeQuote->FlushBaskets();
    m_pTreeQuote->Write( "", TObject::kOverwrite );
    m_pTreeQuote.reset();

    m_pTreeTrade->SetDirectory( m_pFile.get() );
    m_pTreeTrade->FlushBaskets();
    m_pTreeTrade->Write( "", TObject::kOverwrite );
    m_pTreeTrade.reset();
  }

};

using pSecurity_t = std::shared_ptr<Security>;
using mapSecurity_t = std::map<std::string,pSecurity_t>;
using fSecurity_t = std::function<void(pSecurity_t)>;
using fRetrievalDone_t = std::function<void()>;
using fDone_t = std::function<void()>;

// ==========

struct end_check {

  using fTest_t = std::function<bool()>;
  boost::asio::steady_timer m_timer;
  fTest_t m_fTest;

  end_check( boost::asio::io_context& context, fTest_t&& fTest )
  : m_timer( context )
  , m_fTest( std::move( fTest ) )
  {
    wait();
  }

  void wait() {
    m_timer.expires_after( std::chrono::seconds( 5 ) );
    m_timer.async_wait( [this]( const boost::system::error_code& error ){
      if ( error ) {
        std::cout << "wait error: " << error.message() << std::endl;
      }
      else {
        if ( m_fTest() ) {
          wait();
        }
        else {
          //std::cout << "wait done" << std::endl;
        }
      }
    });
  }

};

// ==========

class Symbols {
public:

  Symbols(
    boost::asio::io_context& context
  , bool& bDone, double dblMinPrice
  , setNames_t&& setNames
  , fSecurity_t&& fSecurity
  , fDone_t&& fDone
  )
  : m_context( context )
  , m_bDone( bDone )
  , m_setIgnoreNames( std::move( setNames ) )
  , m_nEndStateLastAcquire {}
  , m_bCheckProcessing( false )
  , m_ec (
      m_context,
      [this]()->bool{
        return Processing();
      })
  {

    m_dblMinPrice = dblMinPrice;
    m_fSecurity = std::move( fSecurity );
    m_fDone = std::move( fDone );

    m_piqfeed = ou::tf::iqfeed::Provider::Factory();
    m_piqfeed->OnConnected.Add( MakeDelegate( this, &Symbols::HandleConnected ) );
    m_piqfeed->Connect();

  }

  void Statistics() {
    std::cout
      << m_countSymbols << " symbols processed, "
      << m_countNonZeroCommonShares
      << " with outstanding shares, "
      << m_countMinimumPrice
      << " with minimum price of $" << m_dblMinPrice
      << std::endl;
  }

protected:

  setNames_t m_setIgnoreNames;

private:

  boost::asio::io_context& m_context;

  end_check m_ec;

  double m_dblMinPrice;
  fSecurity_t m_fSecurity;
  fDone_t m_fDone;

  using pIQFeed_t = ou::tf::iqfeed::Provider::pProvider_t;
  pIQFeed_t m_piqfeed;

  size_t m_countSymbols {};
  size_t m_countNonZeroCommonShares {};
  size_t m_countMinimumPrice {};

  std::mutex m_mutex;

  bool& m_bDone;
  bool m_bCheckProcessing;
  size_t m_nEndStateLastAcquire;

  using key_t = ou::tf::iqfeed::SymbolLookup::key_t;

  mapSecurity_t m_mapSecurity;

  void HandleConnected( int ) {
    //std::cout << "connected" << std::endl;
    m_piqfeed->SymbolList(
      setListedMarket, setSecurityTypes,
      [this](const std::string& sSymbol, key_t keyListedMarket){
        //std::cout << sSymbol << std::endl;
        if ( m_setIgnoreNames.end() != m_setIgnoreNames.find( sSymbol ) ) {
          //std::cout << "ignored " << sSymbol << std::endl;
        }
        else {
          bool bGetFundamentals( false );
          {
            const std::string sListedMarket( m_piqfeed->ListedMarket( keyListedMarket ) );
            std::scoped_lock<std::mutex> lock( m_mutex );
            m_mapSecurity.emplace( sSymbol, std::make_shared<Security>( sSymbol, sListedMarket ) );
            m_bCheckProcessing = true;
            if ( nMaxInTransit > m_mapAcquire.size() ) {
              bGetFundamentals = true;
            }
            m_countSymbols++;
          }
          if ( bGetFundamentals ) GetFundamentals();
        }
      },
      [this](){
        //std::cout << "added " << m_countSymbols << " symbols" << std::endl;
      }
    );
  }

  using pAcquireFundamentals_t = std::shared_ptr<ou::tf::AcquireFundamentals>;

  // assumes single thread
  pAcquireFundamentals_t m_pAcquireFundamentals_burial;

  struct Acquire {
    pSecurity_t pSecurity;
    pAcquireFundamentals_t pAcquireFundamentals;
    Acquire( pSecurity_t pSecurity_ ): pSecurity( pSecurity_ ) {}
  };

  using mapAcquire_t = std::map<std::string,Acquire>;
  mapAcquire_t m_mapAcquire;

  void GetFundamentals() {

    using pWatch_t = ou::tf::Watch::pWatch_t;
    using Fundamentals = ou::tf::Watch::Fundamentals;
    using pInstrument_t = ou::tf::Instrument::pInstrument_t;

    if ( 0 < m_mapSecurity.size() ) {

      std::scoped_lock<std::mutex> lock( m_mutex );

      mapSecurity_t::iterator iterSecurity = m_mapSecurity.begin();
      pSecurity_t pSecurity = iterSecurity->second;
      m_mapSecurity.erase( iterSecurity );

      const std::string& sName( pSecurity->sName );

      pInstrument_t pInstrument = std::make_shared<ou::tf::Instrument>( sName );
      pInstrument->SetAlternateName( ou::tf::Instrument::eidProvider_t::EProviderIQF, sName );
      pWatch_t pWatch = std::make_shared<ou::tf::Watch>( pInstrument, m_piqfeed );

      mapAcquire_t::iterator iterAcquire
        = m_mapAcquire.insert(
            m_mapAcquire.begin(),
            mapAcquire_t::value_type( sName, Acquire( pSecurity ) ) );

      iterAcquire->second.pAcquireFundamentals
        = ou::tf::AcquireFundamentals::Factory(
          std::move( pWatch ),
          [this,iterAcquire]( pWatch_t pWatch ){
            if ( pWatch ) {
              const Fundamentals& fundamentals( pWatch->GetFundamentals() );
              Security& security( *iterAcquire->second.pSecurity.get() );

              if ( 0 < fundamentals.dblCommonSharesOutstanding ) {
                m_countNonZeroCommonShares++;

                if ( m_dblMinPrice < fundamentals.dbl52WkLo ) {

                  m_countMinimumPrice++;

                  security.dblAssets = fundamentals.dblAssets;
                  security.dblLiabilities = fundamentals.dblLiabilities;
                  security.nAverageVolume = fundamentals.nAverageVolume;
                  security.dblPriceEarnings = fundamentals.dblPriceEarnings;
                  security.dblCommonSharesOutstanding = fundamentals.dblCommonSharesOutstanding;
                  security.nSIC = fundamentals.nSIC;
                  security.nNAICS = fundamentals.nNAICS;

                  security.RdafInit();

                  pSecurity_t pSecurity = iterAcquire->second.pSecurity;
                  m_fSecurity( pSecurity );
                }
              }
            }
            else {
              std::cout << "GetFundamentals() error acquiring " << iterAcquire->first << std::endl;
            }

            m_pAcquireFundamentals_burial = std::move( iterAcquire->second.pAcquireFundamentals );
            {
              std::scoped_lock<std::mutex> lock( m_mutex );
              m_mapAcquire.erase( iterAcquire );
            }

            GetFundamentals();  // this is outside of construction thread
          }
        );
      iterAcquire->second.pAcquireFundamentals->Start();
    }
  }

  bool Processing() {
    bool bProcessing( true );
    if ( m_bCheckProcessing ) {
      if ( 0 == m_mapSecurity.size() ) { // assumes more threads than #problem symbols
        const size_t n = m_mapAcquire.size();
        if ( 0 == n ) {
          m_nEndStateLastAcquire = 0;
          bProcessing = false;
          m_bDone = true;
          if ( m_fDone ) m_fDone();
        }
        else {
          //if ( n != m_nEndStateLastAcquire ) {
            std::scoped_lock<std::mutex> lock( m_mutex );
            std::cout << "** waiting on:";
            for ( const mapAcquire_t::value_type& vt: m_mapAcquire ) {
              std::cout << " " << vt.first;
            }
            std::cout << std::endl;
            m_nEndStateLastAcquire = n;
          //}
        }
      }
    }
    return bProcessing;
  }

};

// ==========

// alternative to this might have been HistoryBulkQuery
class RetrieveTicks: ou::tf::iqfeed::HistoryQuery<RetrieveTicks> {
  friend ou::tf::iqfeed::HistoryQuery<RetrieveTicks>;
public:

  using fConnected_t = std::function<void()>;

  RetrieveTicks( fConnected_t&& fConnected )
  : ou::tf::iqfeed::HistoryQuery<RetrieveTicks>()
  {
    assert( fConnected );
    m_fConnected = std::move( fConnected );
    Connect();
  }

  virtual ~RetrieveTicks() {
    Disconnect();
  }

  using fTick_t = std::function<void(const ou::tf::iqfeed::HistoryStructs::TickDataPoint& )>;
  using fDone_t = std::function<void(bool)>;

  void RequestNDaysOfTicks(
    const std::string& sName
  , unsigned int nDays
  , fTick_t&& fTick, fDone_t&& fDone
  ) {
    assert( fTick );
    assert( fDone );
    m_fTick = std::move( fTick );
    m_fDone = std::move( fDone );
    RetrieveNDaysOfDataPoints( sName, nDays );
  }

protected:
  void OnHistoryConnected() {
    std::cout << "History Connected" << std::endl;
    m_fConnected();
  }

  void OnHistoryDisconnected() {
    std::cout << "History DisConnected" << std::endl;
  }

  void OnHistoryError( size_t e ) {
    std::cout << "RetrieveTicks::OnHistoryError " << e << " *********" << std::endl;
  }

  void OnHistoryTickDataPoint( TickDataPoint* pDP ) {
    m_fTick( *pDP );
    ReQueueTickDataPoint( pDP );
  };

  void OnHistoryRequestDone( bool bStatus ) {
    m_fDone( bStatus );
  };
private:
  fConnected_t m_fConnected;
  fTick_t m_fTick;
  fDone_t m_fDone;
};

// ==========

class ControlTickRetrieval {
public:
  ControlTickRetrieval(
    unsigned int nDays
  , unsigned int nSimultaneousRetrievals
  , fSecurity_t&& fSecurity
  , fRetrievalDone_t&& fRetrievalDone
  )
  : m_nDays( nDays )
  , m_nSimultaneousRetrievals( nSimultaneousRetrievals )
  , m_fSecurity( std::move( fSecurity ) )
  , m_fRetrievalDone( std::move( fRetrievalDone ) )
  {
    assert( m_fSecurity );
    assert( m_fRetrievalDone );

    futureStart = promiseStart.get_future();

    for ( uint32_t count = 0; count < m_nSimultaneousRetrievals; count++ ) {
      m_vRetrieveTicks_Avail.emplace_back(
        std::make_shared<RetrieveTicks>(
          [this](){
            m_countStarted++;
            if ( m_nSimultaneousRetrievals == m_countStarted ) {
              promiseStart.set_value( 1 );
            }
          } )
        );
    }

    futureStart.wait();
    assert( m_nSimultaneousRetrievals == m_countStarted );  // assumes sync startup, use future/promise if async
  }

  void Retrieve( pSecurity_t pSecurity ) {
    {
      std::scoped_lock<std::mutex> lock( m_mutex );
      m_mapSecurity_Waiting.emplace( pSecurity->sName, pSecurity );
    }
    StartRetrieval();
  }

  void CheckDone() {
    std::cout
      << "CheckDone "
      << nErrors
      << "," << m_mapSecurity_Waiting.size()
      << "," << m_vRetrieveTicks_Avail.size()
      ;
    if ( 0 != m_mapRetrieveTicks.size() ) {
      //std::scoped_lock<std::mutex> lock( m_mutex ); // can't lock, already in a lock
      for ( const mapRetrieveTicks_t::value_type& vt: m_mapRetrieveTicks ) {
        auto pSecurity = vt.second.pSecurity;
        std::cout
          << "," << pSecurity->sName
          << ":" << pSecurity->nTicks << " ticks"
          ;
      }
      if ( 0 < m_mapRetrieveTicks.size() ) {
        std::cout << "," << m_mapRetrieveTicks.size() << " in progress";
      }
    }
    std::cout << std::endl;
    if ( 0 == m_mapSecurity_Waiting.size() ) {
      if ( m_nSimultaneousRetrievals == m_vRetrieveTicks_Avail.size() ) {
        if ( 0 == m_mapRetrieveTicks.size() ) {
          m_fRetrievalDone();
        }
      }
    }
  }

protected:
private:

  unsigned int m_nDays;
  unsigned int m_nSimultaneousRetrievals;
  fSecurity_t m_fSecurity;
  fRetrievalDone_t m_fRetrievalDone;

  std::mutex m_mutex;

  int m_countStarted {};
  using pRetrieveTicks_t = std::shared_ptr<RetrieveTicks>;
  using vRetrieveTicks_t = std::vector<pRetrieveTicks_t>;
  vRetrieveTicks_t m_vRetrieveTicks_Avail;

  std::promise<int> promiseStart;
  std::future<int> futureStart;

  mapSecurity_t m_mapSecurity_Waiting;

  struct Running {
    pRetrieveTicks_t pRetrieveTicks;
    pSecurity_t pSecurity;
    Running( pRetrieveTicks_t pRetrieveTicks_, pSecurity_t pSecurity_ )
    : pRetrieveTicks( pRetrieveTicks_ ), pSecurity( pSecurity_ )
    {}
  };

  using mapRetrieveTicks_t = std::map<std::string, Running>;
  mapRetrieveTicks_t m_mapRetrieveTicks;

  void StartRetrieval() {
    std::scoped_lock<std::mutex> lock( m_mutex );
    //std::cout << "StartRetrieval: "
    //  << m_vRetrieveTicks_Avail.size()
    //  << "," << m_mapSecurity_Waiting.size()
    //  << std::endl;
    if ( 0 < m_vRetrieveTicks_Avail.size() ) {
      if ( 0 < m_mapSecurity_Waiting.size() ) {

        pRetrieveTicks_t pRetrieveTicks = m_vRetrieveTicks_Avail.back();
        m_vRetrieveTicks_Avail.pop_back();
        mapSecurity_t::iterator iterSecurity = m_mapSecurity_Waiting.begin();
        pSecurity_t pSecurity = iterSecurity->second;
        m_mapSecurity_Waiting.erase( iterSecurity );

        auto result = m_mapRetrieveTicks.emplace( pSecurity->sName, Running( pRetrieveTicks, pSecurity ) );
        assert( result.second );

        pRetrieveTicks->RequestNDaysOfTicks(
          pSecurity->sName, m_nDays,
          [pSecurity](const ou::tf::iqfeed::HistoryStructs::TickDataPoint& tdp){  // fTick_t

            pSecurity->nTicks++;

            auto mu = (tdp.DateTime - ptime(boost::gregorian::date(1970,1,1))).total_microseconds();
            double dbl_mu = (double)mu;
            double shifted_mu = dbl_mu / ( 1000.0 * 1000.0 * 1000.0 );

            Security::QuoteForBranch& qfb( pSecurity->m_branchQuote );

            qfb.time = shifted_mu;
            qfb.ask = tdp.Ask;
            qfb.askvol = 0;
            qfb.bid = tdp.Bid;
            qfb.bidvol = 0;

            const double mid = ( tdp.Ask + tdp.Bid ) / 2.0;

            pSecurity->m_pTreeQuote->Fill();

            Security::TradeForBranch& tfb( pSecurity->m_branchTrade );

            const double price = tdp.Last;
            const int64_t volume = tdp.LastSize;

            tfb.time = shifted_mu;
            tfb.price = price;
            tfb.vol = volume;

            if ( mid == price ) {
              tfb.direction = 0;
            }
            else {
              tfb.direction = ( mid < price ) ? volume : -volume;
            }

            pSecurity->m_pTreeTrade->Fill();

          },
          [this,iter=result.first]( bool bStatus ){ // fDone_t
            if ( !bStatus ) ++nErrors;
            pSecurity_t pSecurity = iter->second.pSecurity;
            m_fSecurity( pSecurity );
            {
              std::scoped_lock<std::mutex> lock( m_mutex );
              m_vRetrieveTicks_Avail.push_back( iter->second.pRetrieveTicks );
              m_mapRetrieveTicks.erase( iter );
            }
            StartRetrieval();
          });
      }
      else {
        assert( 0 == m_mapSecurity_Waiting.size() );
        CheckDone();
      }
    }
  }

};

// ==========

void StartRdaf( const std::string& sFileName ) {

  int argc {};
  char** argv = nullptr;

  std::string sPath( sFileName + ".root" );

  if ( boost::filesystem::exists( sPath ) ) {
    boost::filesystem::remove( sPath );
  }

  m_prdafApp = std::make_unique<TRint>( sPath.c_str(), &argc, argv );
  ROOT::EnableImplicitMT();
  ROOT::EnableThreadSafety();

  // open file after directory scan, so it is not included in the list
  m_pFile = std::make_shared<TFile>(
    sPath.c_str(),
    "RECREATE",
    "tradeframe rdaf/dl quotes, trades"
  );

}

// ==========

void clean_up() {

    m_pTreeStatistics->FlushBaskets();

    if ( m_pFile ) { // performed at exit to ensure no duplication in file
      //m_pFile->Flush();  // probably not needed.
      m_pFile->Write();
      m_pFile->Close();
      m_pFile.reset();
    }

    if ( m_prdafApp ) {
      m_prdafApp.reset();
    }
}

// ==========

void signal_handler(
  const boost::system::error_code& error_code,
  int signal_number
) {
  if ( !error_code ) {
    std::cout
      << "signal(" << error_code << "): "
      << error_code.message() << ", clean up"
      << std::endl;
    clean_up();
    std::terminate();
}  }


// ==========

int main( int argc, char* argv[] ) {

  boost::asio::io_context m_context;

  std::unique_ptr<boost::asio::executor_work_guard<boost::asio::io_context::executor_type> > m_pWork;
  m_pWork
    = std::make_unique<boost::asio::executor_work_guard<boost::asio::io_context::executor_type> >( boost::asio::make_work_guard( m_context ) );

  struct StatsForBranch {
    double pe;
    size_t volume;
    double assets;
    double liabilities;
    double shares;
    size_t ticks;
    size_t sic;
    size_t naics;

    std::string sSecurityName;
    std::string sSecurityListedMarket;

    void Set(const Security& security ) {
      pe = security.dblPriceEarnings;
      volume = security.nAverageVolume;
      assets = security.dblAssets;
      liabilities = security.dblLiabilities;
      shares = security.dblCommonSharesOutstanding;
      ticks = security.nTicks;
      sic = security.nSIC;
      naics = security.nNAICS;

      sSecurityName = security.sName;
      sSecurityListedMarket = security.sListedMarket;
    }
  } m_branchStatistics;

  TBranch* pBranchStatistics;
  TBranch* pBranchName;
  TBranch* pBranchMarket;

  config::Choices choices;
  if ( config::Load( "rdaf/download.cfg", choices ) ) {

    std::cout << "choices min price:  " << choices.m_dblMinPrice << std::endl;
    std::cout << "choices #days:      " << choices.m_nDays << std::endl;
    std::cout << "choices retrievals: " << choices.m_nSimultaneousRetrievals << std::endl;

    for ( const config::vName_t::value_type& vt: choices.m_vListedMarket ) {
      std::cout << "choices listed market: " << vt << std::endl;
      setListedMarket.emplace( vt );
    }

    for ( const config::vName_t::value_type& vt: choices.m_vSecurityType ) {
      std::cout << "choices security type: " << vt << std::endl;
      setSecurityTypes.emplace( vt );
    }

    setNames_t setIgnoreNames;

    for ( const config::vName_t::value_type& vt: choices.m_vIgnoreNames ) {
      std::cout << "choices ignoring:      " << vt << std::endl;
      setIgnoreNames.emplace( vt );
    }

    StartRdaf( "rdaf_dl" );

    m_pTreeStatistics = std::make_shared<TTree>(
      "statistics", "symbol statistics", 99, m_pFile.get()
    );
    if ( !m_pTreeStatistics ) {
      BOOST_LOG_TRIVIAL(error) << "problems m_pTreeStatistics";
    }
    else {
      pBranchStatistics
        = m_pTreeStatistics->Branch(
          "statistics", &m_branchStatistics,
          "pe/D:volume/l:assets/D:liabilities/D:shares/D:ticks/l:sic/l:naics/l"
        );
      pBranchName = m_pTreeStatistics->Branch( "name", &m_branchStatistics.sSecurityName );
      pBranchMarket = m_pTreeStatistics->Branch( "market", &m_branchStatistics.sSecurityListedMarket );
    }

    // https://www.boost.org/doc/libs/1_79_0/doc/html/boost_asio/reference/signal_set.html
    boost::asio::signal_set signals( m_context, SIGINT, SIGTERM, SIGQUIT );
    signals.async_wait( signal_handler );

    bool bSelectionComplete( false );

    ControlTickRetrieval control(
      choices.m_nDays,
      choices.m_nSimultaneousRetrievals,
      [&m_branchStatistics,&m_context]( pSecurity_t pSecurity ){ // fSecurity_t, finished securities
        //std::cout << pSecurity->sName << " history processed." << std::endl;
        const Security& security( *pSecurity );
        std::cout
          << security.sName << ","
          << security.sListedMarket << ","
          << security.dblCommonSharesOutstanding << ","
          //<< pWatch->LastTrade().Price() << ","
          << security.nAverageVolume << ","
          //<< fundamentals.dbl52WkLo
          << security.nTicks
          << std::endl;
        m_context.post( // run in main thread
          [pSecurity,&m_branchStatistics](){
            pSecurity->RdafDirectory(); // set directory, flush, and remove from memory

            m_branchStatistics.Set( *pSecurity );
            m_pTreeStatistics->Fill();
          }
        );
      },
      [&m_pWork,&signals,&bSelectionComplete](){ // fRetrievalDone_t
        if ( bSelectionComplete ) {
          signals.clear();
          signals.cancel();
          m_pWork.reset();
        }
      }
      );

    Symbols symbols(
      m_context
    , bSelectionComplete
    , choices.m_dblMinPrice
    , std::move( setIgnoreNames )
    , [&control]( pSecurity_t pSecurity ) { // fSecurity_t
        //std::cout << "Symbols: " << pSecurity->sName << " sent to history" << std::endl;
        control.Retrieve( pSecurity );
      }
    , [&control](){ // fDone_t
        std::cout << "Symbols: CheckDone" << std::endl;
        control.CheckDone();
      }
    );

    m_context.run();  // falls through when m_pWork reset

    symbols.Statistics();

    clean_up();

  }

  return 0;
}
