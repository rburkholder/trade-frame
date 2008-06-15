#pragma once
#include "k:\data\projects\genesis\v8\gensrc\api\inc\gtstock.h"
#include "GTSessionX.h"
#include "GTOrderX.h"
#include <map>
#include "DatedDatum.h"

#include "FastDelegate.h"
using namespace fastdelegate;

class CGTOrderX;

class CGTStockX :
  public GTStock {

public:
  CGTStockX(CGTSessionX &session, LPCSTR pszStock);
  ~CGTStockX(void);

  typedef FastDelegate1<const CTrade &> OnPrintHandler;
  void SetOnPrintHandler( OnPrintHandler function ) {
    OnPrint = function;
  }
  typedef FastDelegate1<const CQuote &> OnQuoteLevel1Handler;
  void SetOnQuoteLevel1Handler( OnQuoteLevel1Handler function ) {
    OnQuoteLevel1 = function;
  }
  typedef FastDelegate1<const CMarketDepth &> OnLevel2Handler;
  void SetOnLevel2Handler( OnLevel2Handler function ) {
    OnLevel2 = function;
  }
  typedef FastDelegate3<double, char, int> OnOpenPositionHandler;
  void SetOnOpenPositionHandler( OnOpenPositionHandler function ) {
    OnOpenPosition = function;
  }

private:

  CString sCallbackText;

  OnQuoteLevel1Handler OnQuoteLevel1;
  OnPrintHandler OnPrint;
  OnLevel2Handler OnLevel2;
  OnOpenPositionHandler OnOpenPosition;

  typedef std::pair<long, CGTOrderX*> pairOrder;
  std::map<long, CGTOrderX*>::iterator iterOrder;
  std::map<long, CGTOrderX*> mapOrder;

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
