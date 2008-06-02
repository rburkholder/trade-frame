#include "StdAfx.h"
#include "Order.h"

COrder::COrder(void) {
}

COrder::COrder( 
    CInstrument *instrument,
    OrderType::enumOrderType eOrderType,
    OrderSide::enumOrderSide eOrderSide, 
    unsigned long nOrderQuantity,
    ptime dtOrderSubmitted
    ) :
    m_pInstrument( instrument ), m_eOrderType( eOrderType ),
    m_eOrderSide( eOrderSide ), m_nOrderQuantity( nOrderQuantity ),
    m_dtOrderSubmitted( dtOrderSubmitted ),
    m_dblPrice1( 0 ), m_dblPrice2( 0 )
{
}

COrder::COrder( 
    CInstrument *instrument,
    OrderType::enumOrderType eOrderType,
    OrderSide::enumOrderSide eOrderSide, 
    double dblPrice1,
    unsigned long nOrderQuantity,
    ptime dtOrderSubmitted
    ) :
    m_pInstrument( instrument ), m_eOrderType( eOrderType ),
    m_eOrderSide( eOrderSide ), m_nOrderQuantity( nOrderQuantity ),
    m_dtOrderSubmitted( dtOrderSubmitted ), 
    m_dblPrice1( dblPrice1 ), m_dblPrice2( 0 )
{
}

COrder::COrder( 
    CInstrument *instrument,
    OrderType::enumOrderType eOrderType,
    OrderSide::enumOrderSide eOrderSide, 
    double dblPrice1, double dblPrice2,
    unsigned long nOrderQuantity,
    ptime dtOrderSubmitted
    ) :
    m_pInstrument( instrument ), m_eOrderType( eOrderType ),
    m_eOrderSide( eOrderSide ), m_nOrderQuantity( nOrderQuantity ),
    m_dtOrderSubmitted( dtOrderSubmitted ), 
    m_dblPrice1( dblPrice1 ), m_dblPrice2( dblPrice2 )
{
}

COrder::~COrder(void) {
}
