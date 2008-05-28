#include "StdAfx.h"
#include "Order.h"

COrder::COrder(void) {
}

COrder::COrder( 
    CProviderInterface *provider, 
    CInstrument *instrument,
    //const std::string &sSymbol, 
    OrderSide::enumOrderSide eOrderSide, 
    unsigned long nOrderQuantity,
    ptime dtOrderSubmitted
    ) :
  m_pProvider( provider ), m_pInstrument( instrument ), 
    m_eOrderSide( eOrderSide ), m_nOrderQuantity( nOrderQuantity ),
    m_dtOrderSubmitted( dtOrderSubmitted )
{
}

COrder::~COrder(void) {
}
