#pragma once


// CVisibleRow

#include "Colour.h"
#include "VisibleElement.h"

#include <vector>
using namespace std;

class CVisibleRow 
  //: public CWnd 
{
	//DECLARE_DYNAMIC(CVisibleRow)

public:
	CVisibleRow();
	virtual ~CVisibleRow();
  void Create( int RowHeight, CPoint &Origin, CWnd *pParentWnd, bool bIsHeader );
  static int RowWidth();

  CVisibleElement m_veAcct1PL;
  CVisibleElement m_veAcct2PL;
  CVisibleElement m_veAcctPL;

  CVisibleElement m_veAcct1BidPending;
  CVisibleElement m_veAcct2BidPending;
  CVisibleElement m_veBidQuan;

  CVisibleElement m_vePrice;

  CVisibleElement m_veAskQuan;
  CVisibleElement m_veAcct1AskPending;
  CVisibleElement m_veAcct2AskPending;

  CVisibleElement m_veTicks; 
  CVisibleElement m_veVolume;

  CVisibleElement m_veTickBuyVolume;
  CVisibleElement m_veTickSellVolume;

  CVisibleElement m_veStaticIndicators;
  CVisibleElement m_veDynamicIndicators;

  void HighlightBid( bool b );
  void HighlightAsk( bool b );
  void HighlightPrice( bool b );

private:
  COLORREF crAccount1;
  COLORREF crAccount2;
  COLORREF crPrice;
  COLORREF crColumnHeader;
  COLORREF crPriceHighlight;
  COLORREF crBidHighlight;
  COLORREF crAskHighlight;

  std::vector<CVisibleElement *> m_rElements;

  static int ElementWidths[];
  static char* ElementColumnNames[];

  void CreateElement( CVisibleElement *pElement, bool bIsHeader,
                                int yPos, int *pxPos, int Width, int Height,
                                DWORD dwStyle,CWnd *pParentWnd );


protected:
//	DECLARE_MESSAGE_MAP()

};


