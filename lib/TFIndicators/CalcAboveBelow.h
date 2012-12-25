/************************************************************************
 * Copyright(c) 2011, One Unified. All rights reserved.                 *
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

#pragma once

// From HDF5 file, calculates statistics for price range above open and below open

#include <string>
#include <map>

#include <TFTrading/ProviderInterface.h>
#include <TFTrading/Portfolio.h>
#include <TFTrading/Symbol.h>
#include <TFTrading/Instrument.h>

using namespace ou::tf;

class AboveBelowInstrument {
public:
  AboveBelowInstrument( void ) {}; 
  ~AboveBelowInstrument( void ) {}; 
protected:
private:
};

class CAboveBelowUnderlying : public AboveBelowInstrument {
public:
  CAboveBelowUnderlying( void ) {};
  ~CAboveBelowUnderlying( void ) {};
protected:
private:
};

class CAboveBelowOption : public AboveBelowInstrument {
public:
  CAboveBelowOption( void ) {};
  ~CAboveBelowOption( void ) {};
protected:
private:
};

class CalcAboveBelow {
public:
  CalcAboveBelow( Instrument::pInstrument_t pInstrument, ProviderInterfaceBase* pDataProvider, ProviderInterfaceBase* pExecutionProvider );
  ~CalcAboveBelow(void);
  void Start( void );
  void Stop( void );
protected:
//  void HandleUpdatePortfolioRecord( CPortfolio::UpdatePortfolioRecord_t );
  void HandleTrade( const ou::tf::Trade &trade );
private:
  Instrument::pInstrument_t m_pInstrument;
  ProviderInterfaceBase* m_pDataProvider;
  ProviderInterfaceBase* m_pExecutionProvider;

  //std::string m_sSymbolName;

  double m_dblLast;

  typedef std::map<std::string, AboveBelowInstrument*> map_instruments_t;
  map_instruments_t m_mapInstruments;

};
