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
 * Project: Dividend
 * Created: April 1, 2022  19:09
 */

#include <TFTrading/TradingEnumerations.h>

#include <TFIQFeed/InMemoryMktSymbolList.h>

#include "Process.hpp"

int main( int argc, char* argv[] ) {

  ou::tf::iqfeed::InMemoryMktSymbolList list;

  std::cout << "loading started ..." << std::endl;

  list.LoadFromFile( "../symbols.ser" );

  std::cout << "loading done" << std::endl;

  using vExchanges_t = std::vector<std::string>;
  vExchanges_t vExchanges;
  vExchanges = {
    "NYSE"
  , "NYSE,NYSE_ARCA"
  , "NYSE_AMERICAN"
  , "NASDAQ"
//  , "NASDAQ,NCM"
  , "NASDAQ,NGM"
  , "NASDAQ,NGSM"
//  , "NASDAQ,OTC"
  };

  using dividend_t = Process::dividend_t;
  using vSymbols_t = Process::vSymbols_t;
  vSymbols_t vSymbols;

  list.SelectSymbolsByExchange(
    vExchanges.begin(), vExchanges.end(),
    [&vSymbols](const ou::tf::iqfeed::InMemoryMktSymbolList::trd_t trd){
      if ( ou::tf::iqfeed::ESecurityType::Equity == trd.sc ) {
        //std::cout << trd.sSymbol << std::endl;
        vSymbols.push_back( dividend_t( trd.sSymbol ) );
      }
    }
    );

  std::cout << "#symbols=" << vSymbols.size() << std::endl;

  Process process( vSymbols );
  process.Wait();

  for ( vSymbols_t::value_type& vt: vSymbols ) {
    if ( 7.0 < vt.yield ) {
      std::cout
        << vt.sExchange
        << "," << vt.sSymbol
        << ",rate," << vt.rate
        << ",yield," << vt.yield
        << ",amount," << vt.amount
        << ",vol," << vt.nAverageVolume
        << ",exdiv," << vt.dateExDividend
        << std::endl;
    }
  }

  return 0;
}

/*

Run: 2022/04/01

NASDAQ=34372
NASDAQ,NCM=2255
NASDAQ,NGM=1652
NASDAQ,NGSM=1791
NASDAQ,OTC=15847

NYSE=8458
NYSE,NYSE_ARCA=13965
NYSE_AMERICAN=373

TSE=2702


== Market Symbol Type and Count ==
BONDS=4604
CALC=0
EQUITY=60063
FOPTION=742073
FOREX=9962
FORWARD=53
FUTURE=42538
ICSPREAD=2298
IEOPTION=1549678
INDEX=19461
MKTRPT=18703
MKTSTATS=3312
MONEY=1064
MUTUAL=31046
PRECMTL=225
SPOT=52
SPREAD=53840
STRATSPREAD=2322
SWAPS=0
TREASURIES=0

Count Optionables  =2291689
Max Underlying Size=41
cntSIC             =19021
cntNAICS           =0

== Market Names and Count ==
ASXCM=14333
BATS=840
BLOOMBERG=649
BMF=1309
CBOE=159
CBOT=9095
CBOT,CBOTMINI=9469
CBOT,CBOT_GBX=74317
CFE=322
CFTC=8954
CMCB=14
CME=22089
CME,CMEMINI=86988
CME,CME_GBX=157785
CME-FL=1756
COMEX=693
COMEX,COMEX_GBX=41862
CVE=1958
DCE=332
DJ=894
DTN=3462
EEXAG=72
EEXC=4
EEXE=205
EEXN=352
EEXP=14619
ENCOM=6471
ENID=6688
EUREX=81425
FTSE=137
FXCM=44
GRNST=439
ICEEA=7319
ICEEC=77371
ICEEF=13250
ICEENDEX=9140
ICEFC=5052
ICEFU=23208
ICEFU,ICEDAF=616
ICEFU,ICEFANG=24
KBCB=149
KCBOT=700
KCBOT,KCBOT_GBX=5705
LME=49
LPPM=4
LSE=9733
LSE,LSEI=1549
MCX=4733
MDEX=6234
MGCB=105
MGE=111
MGE,MGE_GBX=1089
MGKB=104
NASDAQ=34372
NASDAQ,NCM=2255
NASDAQ,NGM=1652
NASDAQ,NGSM=1791
NASDAQ,OTC=15847
NFX=778
NYMEX=6502
NYMEX,CLEARPORT=1590
NYMEX,NYMEXMINI=4926
NYMEX,NYMEX_GBX=321821
NYSE=8458
NYSE,NYSE_ARCA=13965
NYSE_AMERICAN=373
OPRA=1549678
PK_SHEETS,PK_ARCA=60
PK_SHEETS,PK_CURRENT=6061
PK_SHEETS,PK_GREY=465
PK_SHEETS,PK_IQXPREM=87
PK_SHEETS,PK_IQXPRIME=372
PK_SHEETS,PK_LIMITED=4314
PK_SHEETS,PK_NASDAQ=921
PK_SHEETS,PK_NOINFO=1940
PK_SHEETS,PK_NYSE=451
PK_SHEETS,PK_NYSE_AMEX=127
PK_SHEETS,PK_OTCQB=1286
PK_SHEETS,PK_QXPREM=71
PK_SHEETS,PK_QXPRIME=99
RUSSELL=5
RUSSELL-FL=283
SAFEX=11558
SGX=13448
SGXAC=7611
SMALL=69
TENFORE=7173
TENFORE_LITE=3138
TOCOM=251
TSE=2702
UNKNOWN=0
USDA=166549
WASDE=1452
ZCE=223

*/