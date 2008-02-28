#pragma once
//#include "afxcmn.h"
#include "CtlLvColumns.h"

class CCtlListCtrlBase :   public CListCtrl{
	DECLARE_DYNAMIC(CCtlListCtrlBase)

public:
  CCtlListCtrlBase(void);
  ~CCtlListCtrlBase(void);
  virtual void InitControl( void );
  virtual void DrawColumns( void );
protected:
	DECLARE_MESSAGE_MAP()
  CLVColumns m_ColumnHeaders;
  int m_nRowCount;
private:
};
