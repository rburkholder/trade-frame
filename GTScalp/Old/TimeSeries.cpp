#include "StdAfx.h"
#include "TimeSeries.h"

// CBars

CBars::CBars(void) {
}

CBars::CBars( unsigned int size )
:CTimeSeries<CBar>( size ) {
}

CBars::~CBars( void ) {
  //CTimeSeries<CBar>::~CTimeSeries();
}

// CTrades

CTrades::CTrades(void) {
}

CTrades::CTrades( unsigned int size )
:CTimeSeries<CTrade>( size ) {
}

CTrades::~CTrades( void ) {
  //CTimeSeries<CTrade>::~CTimeSeries();
}

// CQuotes

CQuotes::CQuotes(void) {
}

CQuotes::CQuotes( unsigned int size )
:CTimeSeries<CQuote>( size ) {
}

CQuotes::~CQuotes( void ) {
  //CTimeSeries<CQuote>::~CTimeSeries();
}


// CMarketDepths

CMarketDepths::CMarketDepths() {
}

CMarketDepths::~CMarketDepths() {
}

CMarketDepths::CMarketDepths(unsigned int size) 
: CTimeSeries<CMarketDepth>( size ) {
}