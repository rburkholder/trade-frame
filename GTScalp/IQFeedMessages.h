#pragma once

#include "boost/date_time/posix_time/posix_time.hpp"

using namespace boost::posix_time;
using namespace boost::gregorian;

#include <string>
using namespace std;

// field offsets are 1 based

class CIQFBaseMessage {
public:

  CIQFBaseMessage(void);
  ~CIQFBaseMessage(void);

  //virtual void Assign( const char *str );
  //virtual void Assign( unsigned short nStr, const char *str ); // size of str, and its array
  virtual void Assign( const char *str );

  void EmitFields( void );
  void EmitLine( void );
  const string &Field( unsigned short ); // returns reference to a field (will be sNull or sField );

  double Double( unsigned short );
  int Integer( unsigned short );

protected:
  //int curPos;

  //string m_sMsg;
  const char *m_pStr;  // dynamically assigned string for fast conversion access later
  //unsigned short m_nStr;  // holds size of string, used if we need to reallocate, includes terminating null
  //unsigned short m_nStrMax; // maximum of size of stored string

  string m_sToken;

  unsigned short rOffset[75];  // Offset of field 
  unsigned short rSize[75];  // number of characters in field
  unsigned short cntFieldsFound;

  string sNull;  // always the empty string
  string sField;  // will hold content of selected field

  virtual void Tokenize();

private:

};

//****
class CIQFSystemMessage: public CIQFBaseMessage { // S
public:

  CIQFSystemMessage(void);
  ~CIQFSystemMessage(void);

private:
};

//****
class CIQFTimeMessage: public CIQFBaseMessage { // T
public:

  CIQFTimeMessage(void);
  ~CIQFTimeMessage(void);

  void Assign( const char *str );

  ptime m_dt;
  boost::posix_time::time_duration m_timeMarketOpen, m_timeMarketClose;
  bool m_bMarketIsOpen;
protected:
  
private:
};

//****
class CIQFNewsMessage: public CIQFBaseMessage { // N
public:

  static const unsigned short NDistributor = 2;
  static const unsigned short NStoryId = 3;
  static const unsigned short NSymbolList = 4;
  static const unsigned short NDateTime = 5;
  static const unsigned short NHeadline = 6;

  string m_sDistributor;
  string m_sStoryId;
  string m_sSymbolList;
  string m_sDateTime;
  string m_sHeadline;

  CIQFNewsMessage(void);
  ~CIQFNewsMessage(void);

  void Assign( const char *str );

private:
};

//**** CIQFPricingMessage ( root for CIQFUpdateMessage, CIQFSummaryMessage )
class CIQFPricingMessage: public CIQFBaseMessage { // Q, P
public:

  static const unsigned short QPSymbol = 2;
  static const unsigned short QPLast = 4;
  static const unsigned short QPChange = 5;
  static const unsigned short QPPctChange = 6;
  static const unsigned short QPTtlVol = 7;
  static const unsigned short QPLastVol = 8;
  static const unsigned short QPHigh = 9;
  static const unsigned short QPLow = 10;
  static const unsigned short QPBid = 11;
  static const unsigned short QPAsk = 12;
  static const unsigned short QPBidSize = 13;
  static const unsigned short QPAskSize = 14;
  static const unsigned short QPTick = 15;
  static const unsigned short QPBidTick = 16;
  static const unsigned short QPTradeRange = 17;
  static const unsigned short QPLastTradeTime = 18;
  static const unsigned short QPOpenInterest = 19;
  static const unsigned short QPOpen = 20;
  static const unsigned short QPClose = 21;
  static const unsigned short QPSpread = 22;
  static const unsigned short QPSettle = 24;
  static const unsigned short QPDelay = 25;
  static const unsigned short QPNav = 28;
  static const unsigned short QPMnyMktAveMaturity = 29;
  static const unsigned short QPMnyMkt7DayYld = 30;
  static const unsigned short QPLastTradeDate = 31;
  static const unsigned short QPExtTradeLast = 33;
  static const unsigned short QPNav2 = 36;
  static const unsigned short QPExtTradeChng = 37;
  static const unsigned short QPExtTradeDif = 38;
  static const unsigned short QPPE = 39;
  static const unsigned short QPPctOff30AveVol = 40;
  static const unsigned short QPBidChange = 41;
  static const unsigned short QPAskChange = 42;
  static const unsigned short QPChangeFromOpen = 43;
  static const unsigned short QPMarketOpen = 44;
  static const unsigned short QPVolatility = 45;
  static const unsigned short QPMarketCap = 46;
  static const unsigned short QPDisplayCode = 47;
  static const unsigned short QPPrecision = 48;
  static const unsigned short QPDaysToExpiration = 49;
  static const unsigned short QPPrevDayVol = 50;
  static const unsigned short QPNumTrades = 56;
  static const unsigned short QPFxBidTime = 57;
  static const unsigned short QPFxAskTime = 58;
  static const unsigned short QPVWAP = 59;
  static const unsigned short QPTickId = 60;
  static const unsigned short QPFinStatus = 61;
  static const unsigned short QPSettleDate = 62;

  CIQFPricingMessage(void);
  ~CIQFPricingMessage(void);

  ptime LastTradeTime( void );

private:
};


//**** CIQFUpdateMessage
class CIQFUpdateMessage: public CIQFPricingMessage { // Q
public:

  CIQFUpdateMessage(void);
  ~CIQFUpdateMessage(void);

  //void Assign( const char *str );

  //bool Found() { return bFound; };

protected:
  //bool bFound;

private:
};

//**** CIQFSummaryMessage
class CIQFSummaryMessage: public CIQFPricingMessage { // P
public:

  CIQFSummaryMessage(void);
  ~CIQFSummaryMessage(void);

private:
};


//**** CIQFFundamentalMessage
class CIQFFundamentalMessage: public CIQFBaseMessage { // F
public:

  static const unsigned short FSymbol = 2;
  static const unsigned short FPE = 4;
  static const unsigned short FAveVolume = 5;
  static const unsigned short F52WkHi = 6;
  static const unsigned short F52WkLo = 7;
  static const unsigned short FCalYrHi = 8;
  static const unsigned short FCalYrLo = 9;
  static const unsigned short FDivYld = 10;
  static const unsigned short FDivAmt = 11;
  static const unsigned short FDivRate = 12;
  static const unsigned short FDivPayDate = 13;
  static const unsigned short FDivExDate = 14;
  static const unsigned short FCurYrEPS = 20;
  static const unsigned short FNxtYrEPS = 21;
  static const unsigned short FFiscalYrEnd = 23;
  static const unsigned short FName = 25;
  static const unsigned short FRootOptionSymbols = 26;
  static const unsigned short FPctInst = 27;
  static const unsigned short FBeta = 28;
  static const unsigned short FLeaps = 29;
  static const unsigned short FCurAssets = 30;
  static const unsigned short FCurLiab = 31;
  static const unsigned short FBalShtDate = 32;
  static const unsigned short FLongTermDebt = 33;
  static const unsigned short FCommonShares = 34;
  static const unsigned short FMarketCenter = 39;
  static const unsigned short FFormatCode = 40;
  static const unsigned short FPrecision = 41;
  static const unsigned short FSIC = 42;
  static const unsigned short FVolatility = 43;
  static const unsigned short FSecurityType = 44;
  static const unsigned short FListedMarket = 45;
  static const unsigned short F52WkHiDate = 46;
  static const unsigned short F52WkLoDate = 47;
  static const unsigned short FCalYrHiDate = 48;
  static const unsigned short FCalYrLoDate = 49;
  static const unsigned short FYearEndClose = 50;
  static const unsigned short FBondMaturityDate = 51;
  static const unsigned short FBondCouponRate = 52;
  static const unsigned short FExpirationDate = 53;
  static const unsigned short FStrikePrice = 54;

  CIQFFundamentalMessage(void);
  ~CIQFFundamentalMessage(void);

private:
};
