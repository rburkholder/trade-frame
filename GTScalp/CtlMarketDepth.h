#pragma once

#include "CtlListCtrlBase.h"
#include "Colour.h"

// CCtlMarketDepth

class CCtlMarketDepth : public CCtlListCtrlBase {
	DECLARE_DYNAMIC(CCtlMarketDepth)

public:
	CCtlMarketDepth();
	virtual ~CCtlMarketDepth();
  void InitControl( void );
  //void DrawColumns( void );

  int NumberOfRows() { return GetCountPerPage(); };
  void Resize();
  void AssignRow( 
    int n, Colour::enumColour colour, const char* szMMID, int nSize, double price, int nOwn, const char *pTime );
  void BlankRemainingRows();
protected:
	DECLARE_MESSAGE_MAP()

  int nInsertedRows;
  int ixLastNonBlankRow;
  int ixCurrentRow;

  int nCumulativeCount;
private:
};


