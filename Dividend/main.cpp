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

#include "Config.hpp"
#include "Process.hpp"

int main( int argc, char* argv[] ) {

  const static std::string sConfigFileName( "dividend.cfg" );

  config::Choices choices;

  if ( Load( sConfigFileName, choices ) ) {
  }
  else {
    return EXIT_FAILURE;
  }

  using vSymbols_t = Process::vSymbols_t;
  vSymbols_t vSymbols;

  {
    Process process( choices, vSymbols );
    process.Wait();
  }

  std::cout
    << "symbol,name,exchange,last($),yield(%),ycalc,rate,amount,vol,exdiv,payed,shares,state,option"
    << std::endl;

  for ( vSymbols_t::value_type& vt: vSymbols ) {
    //if ( ( choices.m_dblMinimumYield <= vt.yield ) && ( choices.m_nMinimumVolume <= vt.nAverageVolume ) ) {
    if ( ( choices.m_dblMinimumYield <= vt.yield_calculated ) && ( choices.m_nMinimumVolume <= vt.nAverageVolume ) ) {
      std::cout
               << vt.sSymbol
        << ',' << vt.sCompanyName
        << ',' << vt.sExchange
        << ',' << vt.trade
        << ',' << vt.yield
        << ',' << vt.yield_calculated
        << ',' << vt.rate
        << ',' << vt.amount
        << ',' << vt.nAverageVolume
        << ',' << vt.dateExDividend
        << ',' << vt.datePayed
        << ',' << vt.nSharesOutstanding
        << ',' << vt.sState
        << ',' << vt.sOptionRoots
        << std::endl;
    }
  }

  return EXIT_SUCCESS;
}

// https://dividendhistory.org
// https://www.barchart.com
// https://www.dividend.com

// https://www.nasdaq.com/market-activity/funds-and-etfs/xxxx/dividend-history
