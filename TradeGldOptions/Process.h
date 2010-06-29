/************************************************************************
 * Copyright(c) 2010, One Unified. All rights reserved.                 *
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

#include <vector>
#include <string>
#include <sstream>
#include <map>

#include <LibTimeSeries/DatedDatum.h>

#include <LibIQFeed/IQFeedHistoryQuery.h>  // seems to be a header ordering dependancy
#include <LibIQFeed/IQFeedProvider.h>

#include <LibInteractiveBrokers/IBTWS.h>



//
// ==================
//

class CNakedOption
{
public:
  CNakedOption( double dblStrike );
  CNakedOption( const CNakedOption& rhs );
  virtual ~CNakedOption( void ) {};

  CNakedOption& operator=( const CNakedOption& rhs );

  bool operator< ( const CNakedOption& rhs ) const { return m_dblStrike <  rhs.m_dblStrike; };
  bool operator<=( const CNakedOption& rhs ) const { return m_dblStrike <= rhs.m_dblStrike; };

  double Strike( void ) { return m_dblStrike; };
  void Symbol( CIBSymbol* pSymbol ) { m_pSymbol = pSymbol; };
  CIBSymbol* Symbol( void ) { return m_pSymbol; };

  void HandleQuote( const CQuote& quote );
  void HandleTrade( const CTrade& trade );
  void HandleGreeks( double ImplVol, double Delta, double Gamma, double Vega, double Theta );

  double Bid( void ) { return m_dblBid; };
  double Ask( void ) { return m_dblAsk; };

protected:

  std::string m_sSide;

  double m_dblBid;
  double m_dblAsk;

  double m_dblStrike;
  double m_dblImpliedVolatility;
  double m_dblDelta;
  double m_dblGamma;
  double m_dblVega;
  double m_dblTheta;

  bool m_bWatching;

  CIBSymbol* m_pSymbol;

  std::stringstream m_ss;

private:
};

//
// ==================
//

class CNakedCall: public CNakedOption
{
public:
  CNakedCall( double dblStrike );
  virtual ~CNakedCall( void ) {};
protected:
private:
};

//
// ==================
//

class CNakedPut: public CNakedOption
{
public:
  CNakedPut( double dblStrike );
  virtual ~CNakedPut( void ) {};
protected:
private:
};

//
// ==================
//

class COptionInfo 
{
public:
  COptionInfo( double dblStrike );
  COptionInfo( const COptionInfo& rhs );
  ~COptionInfo( void );

  COptionInfo& operator=( const COptionInfo& rhs );

  bool operator< ( const COptionInfo& rhs ) const { return m_dblStrike <  rhs.m_dblStrike; };
  bool operator<=( const COptionInfo& rhs ) const { return m_dblStrike <= rhs.m_dblStrike; };

  double Strike( void ) { return m_dblStrike; };

  CNakedCall* Call( void ) { return &m_call; };
  CNakedPut* Put( void ) { return &m_put; };

protected:

  CNakedCall m_call;
  CNakedPut m_put;

private:
  std::stringstream m_ss;
  bool m_bWatching;
  double m_dblStrike;
};

//
// ==================
//

class CProcess: 
  public CIQFeedHistoryQuery<CProcess>
{
  friend CIQFeedHistoryQuery<CProcess>;
public:
  CProcess(void);
  ~CProcess(void);

  void IBConnect( void );
  void IBDisconnect( void );

  void IQFeedConnect( void );
  void IQFeedDisconnect( void );

  void StartWatch( void );
  void StopWatch( void );

  void StartTrading( void ) ;
  void StopTrading( void );

protected:

  void OnHistoryConnected( void );
  void OnHistorySummaryData( structSummary* pDP );
  void OnHistoryRequestDone( void );
  void OnHistoryDisconnected( void ) {};

private:

  typedef double strike_t;

  std::string m_sSymbolName;
  long m_contractidUnderlying;
  CIBSymbol *pUnderlying;  // need to make share_ptr

  std::stringstream m_ss;

  CIQFeedProvider m_iqfeed;
  bool m_bIQFeedConnected;

  CIBTWS m_tws;
  bool m_bIBConnected;
  bool m_bWatchingOptions;
  bool m_bTrading;

  CBar m_Bar;  // keep pointer for when data arrives

  std::vector<double> m_vCrossOverPoints;  // has pivots and strikes in order
  std::vector<COptionInfo> m_vStrikes;  // put/call info for each strike
  std::vector<COptionInfo>::iterator m_iterStrikes;

  std::vector<double>::iterator m_iterAboveCrossOver;
  std::vector<double>::iterator m_iterBelowCrossOver;

  std::vector<COptionInfo>::iterator m_iterOILowestWatch;
  std::vector<COptionInfo>::iterator m_iterOIHighestWatch;
  std::vector<COptionInfo>::iterator m_iterOILatestGammaSelect;

  Contract m_contract; // re-usable, persistant contract scratchpad

  int m_nCalls;
  int m_nPuts;

  double m_dblCallPrice;
  double m_dblPutPrice;

  void HandleOnIBConnected( int );
  void HandleOnIBDisconnected( int );

  void HandleOnIQFeedConnected( int );
  void HandleOnIQFeedDisconnected( int );

  void HandleStrikeListing1( const ContractDetails& );  // listing of strikes
  void HandleStrikeListing1Done( void );
  void HandleStrikeListing2( const ContractDetails& );  // underlying contract
  void HandleStrikeListing2Done( void );
  void HandleStrikeListing3( const ContractDetails& );  // symbols for Calls
  void HandleStrikeListing3Done( void );
  void HandleStrikeListing4( const ContractDetails& );  // symbols for puts
  void HandleStrikeListing4Done( void );

  void HandleMainQuote( const CQuote& quote );
  void HandleMainTrade1( const CTrade& trade ); // first trade gives our center oint
  void HandleMainTradeN( const CTrade& trade ); // following trades follow

};
