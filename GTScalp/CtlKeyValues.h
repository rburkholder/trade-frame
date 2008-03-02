#pragma once

#include "RowKeyValues.h"
//#include "CtlLvColumns.h"
#include "CtlListCtrlBase.h"

// CCtlKeyValues

class CCtlKeyValues : public CCtlListCtrlBase {
	DECLARE_DYNAMIC(CCtlKeyValues)

public:

  CCtlKeyValues();
	virtual ~CCtlKeyValues();

  void InitControl();
  //void DrawColumns( void );
  CRowKeyValues* InsertSymbol( const char * szSymbol );

  BOOL memberFxn( UINT id, NMHDR * pTTTStruct, LRESULT * pResult ) { return FALSE; };
  afx_msg void fn( NMHDR * nm, LRESULT * pResult );
  afx_msg void OnLButtonUp( UINT, CPoint );
  afx_msg void OnLButtonDown( UINT, CPoint );

  afx_msg void OnNMCustomdrawKeyvalues(NMHDR *pNMHDR, LRESULT *pResult);

  LPCTSTR GetFullName( int ix );

protected:

  //CLVColumns m_Columns;

  //int m_nRowCount;

  typedef CRowKeyValues* LPROWKEYVALUES;
  std::vector<LPROWKEYVALUES> m_vRowKeyValues;

  DECLARE_MESSAGE_MAP()
public:
};


