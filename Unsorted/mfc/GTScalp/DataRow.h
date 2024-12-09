#pragma once
#pragma warning( disable : 4996 )


#include "DataRowElement.h"
#include "VisibleRow.h"

#include <OUCommon/FastDelegate.h>
using namespace fastdelegate;

class CDataRow {
public:
  CDataRow( unsigned int, double );
  ~CDataRow(void);

  static unsigned int DoubleToInt( double Price, int Multiplier );
  static double IntToDouble( unsigned int Index, int Multiplier );

  CDataRowElement<double> *m_pAcct1PL;
  CDataRowElement<double> *m_pAcct2PL;
  CDataRowElement<double> *m_pAcctPL;
  CDataRowElement<int> *m_pAcct1BidPending;
  CDataRowElement<int> *m_pAcct2BidPending;
  CDataRowElement<int> *m_pBidQuan;
  CDataRowElementPrice *m_pPrice;
  CDataRowElement<int> *m_pAskQuan;
  CDataRowElement<int> *m_pAcct1AskPending;
  CDataRowElement<int> *m_pAcct2AskPending;
  CDataRowElement<int> *m_pTicks;
  CDataRowElement<int> *m_pVolume;
  CDataRowElement<int> *m_pTickBuyVolume;
  CDataRowElement<int> *m_pTickSellVolume;
  CDataRowElementStaticIndicator *m_pStaticIndicators;
  CDataRowElementDynamicIndicator *m_pDynamicIndicators;

  void SetMapToVisibleRow( CVisibleRow *pvr );
  void UnsetMapToVisibleRow( void );

  CVisibleRow *m_pvr;
  int PriceIndex;  // Price * 100 or Price * 1000
  double m_dblPrice;

  void CheckRefresh();

  typedef FastDelegate1<int> OnClickHandler;

  void SetOnAccount1BidRClick( OnClickHandler function ) { OnAccount1BidRClick = function; }
  void SetOnAccount2BidRClick( OnClickHandler function ) { OnAccount2BidRClick = function; }
  void SetOnAccount1AskRClick( OnClickHandler function ) { OnAccount1AskRClick = function; }
  void SetOnAccount2AskRClick( OnClickHandler function ) { OnAccount2AskRClick = function; }
  void SetOnAccount1BidLClick( OnClickHandler function ) { OnAccount1BidLClick = function; }
  void SetOnAccount2BidLClick( OnClickHandler function ) { OnAccount2BidLClick = function; }
  void SetOnAccount1AskLClick( OnClickHandler function ) { OnAccount1AskLClick = function; }
  void SetOnAccount2AskLClick( OnClickHandler function ) { OnAccount2AskLClick = function; }

  void HandleAccount1BidRClick( void );
  void HandleAccount2BidRClick( void );
  void HandleAccount1AskRClick( void );
  void HandleAccount2AskRClick( void );
  void HandleAccount1BidLClick( void );
  void HandleAccount2BidLClick( void );
  void HandleAccount1AskLClick( void );
  void HandleAccount2AskLClick( void );

protected:
private:

  OnClickHandler OnAccount1BidRClick;
  OnClickHandler OnAccount2BidRClick;
  OnClickHandler OnAccount1AskRClick;
  OnClickHandler OnAccount2AskRClick;
  OnClickHandler OnAccount1BidLClick;
  OnClickHandler OnAccount2BidLClick;
  OnClickHandler OnAccount1AskLClick;
  OnClickHandler OnAccount2AskLClick;

  bool bChanged;  // should we change this into a semaphore so multiple uses are auto-sync'd?
  void Redraw();

};
