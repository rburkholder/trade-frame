#pragma once

#include "k:\data\projects\genesis\v8\gensrc\api\inc\gtstock.h"

#include "symbol.h"

//#include "GndtProvider.h"

class CGndtProvider;
class CGndtSymbol :  public GTStock, public CSymbol { // this order might be important to GTSession
  friend class CGndtProvider;
public:
  CGndtSymbol( LPCSTR sSymbolName, CGndtProvider *pProvider );
  virtual ~CGndtSymbol(void);
protected:
  void StartTradeWatch( void );
  void StopTradeWatch( void );
  void StartQuoteWatch( void );
  void StopQuoteWatch( void );
  void StartDepthWatch( void );
  void StopDepthWatch( void );
private:
  virtual int OnGotQuoteLevel1(GTLevel1 *pRcd);
  virtual int OnGotQuotePrint(GTPrint *pRcd);

  virtual int OnGotLevel2Record(GTLevel2 *pRcd);
  virtual int OnGotLevel2Refresh();
  virtual int OnGotLevel2Display();
  virtual int OnGotLevel2Clear(MMID mmid);

  virtual int OnBestBidPriceChanged();
  virtual int OnBestAskPriceChanged();

  virtual int PrePlaceOrder(GTOrder &order);
  virtual int PostPlaceOrder(GTOrder &order);

  virtual int OnSendingOrder(const GTSending &gtsending);
  virtual int OnExecMsgSending(const GTSending &sending);
  virtual int OnExecMsgPending(const GTPending &pending);
  virtual int OnExecMsgTrade(const GTTrade &trade);
  virtual int OnExecMsgCanceling(const GTCancel &cancel);
  virtual int OnExecMsgCancel(const GTCancel &cancel);  // cancelled

  virtual int OnExecMsgErrMsg(const GTErrMsg &errmsg);
  virtual int OnExecMsgOpenPosition(const GTOpenPosition &open);
  virtual int OnExecMsgReject(const GTReject &reject);
  virtual int OnExecMsgRemove(const GTRemove &remove);
  virtual int OnExecMsgRejectCancel(const GTRejectCancel &rejectcancel);

  /*  virtual int OnGotQuotePrintHistory(GTPrint *pRcd);
  virtual int OnGotQuotePrintRefresh();
  virtual int OnGotQuotePrintDisplay();
  virtual int OnGetPrintColor(GTPrint &print);

  virtual int OnExecMsgIOIStatus(const GTIOIStatus &ioi);
*/
//  GTAPI_API double CalcOpenPL(const GTOpenPosition &open, const GTLevel1 &l1);
};
