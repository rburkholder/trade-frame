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
  AssignOrderId();
}

COrder::COrder( 
    CInstrument *instrument,
    OrderType::enumOrderType eOrderType,
    OrderSide::enumOrderSide eOrderSide, 
    unsigned long nOrderQuantity,
    double dblPrice1,
    ptime dtOrderSubmitted
    ) :
    m_pInstrument( instrument ), m_eOrderType( eOrderType ),
    m_eOrderSide( eOrderSide ), m_nOrderQuantity( nOrderQuantity ),
    m_dtOrderSubmitted( dtOrderSubmitted ), 
    m_dblPrice1( dblPrice1 ), m_dblPrice2( 0 )
{
  AssignOrderId();
}

COrder::COrder( 
    CInstrument *instrument,
    OrderType::enumOrderType eOrderType,
    OrderSide::enumOrderSide eOrderSide, 
    unsigned long nOrderQuantity,
    double dblPrice1, double dblPrice2,
    ptime dtOrderSubmitted
    ) :
    m_pInstrument( instrument ), m_eOrderType( eOrderType ),
    m_eOrderSide( eOrderSide ), m_nOrderQuantity( nOrderQuantity ),
    m_dtOrderSubmitted( dtOrderSubmitted ), 
    m_dblPrice1( dblPrice1 ), m_dblPrice2( dblPrice2 )
{
  AssignOrderId();
}

COrder::~COrder(void) {
}

void COrder::AssignOrderId() {
//  try {
    m_nOrderId = m_persistedorderid.GetNextOrderId();
//  }
//  catch (...) {
//    bOrderIdOk = false;
//    std::cout << "CIBTWS::PlaceOrder: Couldn't get the next order key." << std::endl;
//  }
}