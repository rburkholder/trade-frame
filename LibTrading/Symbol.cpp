#include "StdAfx.h"
#include "Symbol.h"

CSymbol::CSymbol( const std::string sName ): 
  m_sSymbolName( sName ), m_nQuoteHandlers( 0 ), m_nTradeHandlers( 0 ), m_nDepthHandlers( 0 )
{
}

CSymbol::~CSymbol(void) {
}

bool CSymbol::AddQuoteHandler(quotehandler_t handler) {
  m_OnQuote.Add( handler );
  ++m_nQuoteHandlers;
  return ( 1 == m_nQuoteHandlers );  // start watch for the symbol
}

bool CSymbol::RemoveQuoteHandler(quotehandler_t handler) {
  assert( 0 < m_nQuoteHandlers );
  m_OnQuote.Remove( handler );
  --m_nQuoteHandlers;
  return ( 0 == m_nQuoteHandlers );  // no more so stop watch
}

bool CSymbol::AddTradeHandler(tradehandler_t handler) {
  m_OnTrade.Add( handler );
  ++m_nTradeHandlers;
  return ( 1 == m_nTradeHandlers ); // start watch on first handler
}

bool CSymbol::RemoveTradeHandler(tradehandler_t handler) {
  assert( 0 < m_nTradeHandlers );
  m_OnTrade.Remove( handler );
  --m_nTradeHandlers;
  return ( 0 == m_nTradeHandlers ); // no more so stop watch
}

bool CSymbol::AddDepthHandler(depthhandler_t handler) {
  m_OnDepth.Add( handler );
  ++m_nDepthHandlers;
  return ( 1 == m_nDepthHandlers );  // when true, start watch
}

bool CSymbol::RemoveDepthHandler(depthhandler_t handler) {
  assert( 0 < m_nDepthHandlers );
  m_OnDepth.Remove( handler );
  --m_nDepthHandlers;
  return ( 0 == m_nDepthHandlers );  // when true, stop watch
}


/*
What is effectively happening, is that myFoo is being "bound" into the newFunc object. Think of it as creating a private variable inside newFunc and sticking myFoo in it. When newFunc is invoked, it will use myFoo as a parameter. 
http://orionedwards.blogspot.com/2006/09/function-pointers-in-cc-and-boostbind.html
*/


/*
class Button {
  typedef boost::signal<void (int x, int y)> OnClick;

public:
  void doOnClick(const OnClick::slot_type& slot);

private:
  OnClick onClick;
};

void Button::doOnClick( const OnClick::slot_type& slot ) {
  onClick.connect(slot);
}

void printCoordinates(long x, long y) {
  std::cout << "(" << x << ", " << y << ")\n";
}

void f(Button& button) {
  button.doOnClick(&printCoordinates);
}
*/