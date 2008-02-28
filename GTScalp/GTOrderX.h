#pragma once

#include "K:\Data\Projects\Genesis\v8\GenSrc\API\INC\GTOrder.h"
#include "K:\Data\Projects\Genesis\v8\GenSrc\API\INC\GTSending.h"
#include "K:\Data\Projects\Genesis\v8\GenSrc\API\INC\GTErrMsg.h"
#include "K:\Data\Projects\Genesis\v8\GenSrc\API\INC\GTTrade.h"
#include "K:\Data\Projects\Genesis\v8\GenSrc\API\INC\GTCancel.h"
#include "K:\Data\Projects\Genesis\v8\GenSrc\API\INC\GTReject.h"
#include "K:\Data\Projects\Genesis\v8\GenSrc\API\INC\GTRemove.h"
#include "K:\Data\Projects\Genesis\v8\GenSrc\API\INC\GTPending.h"
#include "GTStockX.h"

#include "FastDelegate.h"
using namespace fastdelegate;

class CGTStockX;

class CGTOrderX: public GTOrder {
public:
  enum EOrderType { Market, Limit, LimitStop, Stop };
  enum EOrderSide { Buy, Sell, Short };

  CGTOrderX( const CGTStockX *stock, EOrderType ot, EOrderSide os, long cntShares );
  ~CGTOrderX(void);

  typedef FastDelegate7<const char*, const char*, const char*, 
    const char*, const char*, const char*, DWORD_PTR> OnOrderSentHandler;
  void SetOnOrderSentHandler( OnOrderSentHandler function ) {
    OnOrderSent = function;
  }

  typedef FastDelegate1<const char*> OnOrderCompletedHandler;
  void SetOnOrderCompletedHandler( OnOrderCompletedHandler function ) {
    OnOrderComplete = function;
  }

  typedef FastDelegate2<const char*, const char*> OnUpdateOrderStatusHandler;
  void SetOnUpdateOrderStatusHandler( OnUpdateOrderStatusHandler function ) {
    OnUpdateOrderStatus = function;
  }


  // Signals from API as to disposition of processing
  void OnSendingOrder( const GTSending &gtsending );
  void OnTraded( const GTTrade &trade );
  void OnCancelled( const GTCancel &cancel );
  void OnRejected( const GTReject &reject );
  void OnRemoved( const GTRemove &remove );
  void OnPending( const GTPending &pending );
  //void OnErrorMessage( const GTErrMsg &gterrmsg );


  const GTPending &GetPending();

private:
  SYSTEMTIME stCreated;
  EOrderType OrderType;
  EOrderSide OrderSide;

  OnOrderSentHandler OnOrderSent;
  OnOrderCompletedHandler OnOrderComplete;
  OnUpdateOrderStatusHandler OnUpdateOrderStatus;

  DWORD dwTraderSeqNo; // GT session unique, assigned before sent to executor, from OnSendingOrder()
  DWORD dwTicketNo;  // acknowledged by server in OnExecMsgPending()

  GTPending m_pending;

};
