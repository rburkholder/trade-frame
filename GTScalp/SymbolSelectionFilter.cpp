#include "StdAfx.h"
#include "SymbolSelectionFilter.h"

CSymbolSelectionFilter::CSymbolSelectionFilter(void) {
}

CSymbolSelectionFilter::~CSymbolSelectionFilter(void) {
}

CSelectSymbolWithDarvas::CSelectSymbolWithDarvas(void) {
  m_bars.Resize( 20 ); 
}

CSelectSymbolWithDarvas::~CSelectSymbolWithDarvas(void) {
}

void CSelectSymbolWithDarvas::Process( const string &sSymbol ) {
  cout << "Darvas for " << sSymbol << ", " << m_bars.Count() << " bars." << endl;
}

CSelectSymbolWithBollinger::CSelectSymbolWithBollinger(void) {
}

CSelectSymbolWithBollinger::~CSelectSymbolWithBollinger(void) {
}

void CSelectSymbolWithBollinger::Process( const string &sSymbol ) {
  cout << "Bollinger for " << sSymbol << ", " << m_bars.Count() << " bars." << endl;
}

CSelectSymbolWithBreakout::CSelectSymbolWithBreakout(void) {
}

CSelectSymbolWithBreakout::~CSelectSymbolWithBreakout(void) {
}

void CSelectSymbolWithBreakout::Process( const string &sSymbol ) {
  cout << "Breakout for " << sSymbol << ", " << m_bars.Count() << " bars." << endl;
}


