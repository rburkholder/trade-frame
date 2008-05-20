#include "StdAfx.h"
#include "Order.h"

COrder::COrder(void) {
}

COrder::COrder( 
    CProviderInterface *provider, 
    const std::string &sSymbol, 
    enumOrderSide eOrderSide, 
    unsigned long nOrderQuantity,
    ptime dtOrderSubmitted
    ) :
  m_pProvider( provider ), m_sSymbol( sSymbol ), 
    m_eOrderSide( eOrderSide ), m_nOrderQuantity( nOrderQuantity ),
    m_dtOrderSubmitted( dtOrderSubmitted )
{
}

COrder::~COrder(void) {
}
