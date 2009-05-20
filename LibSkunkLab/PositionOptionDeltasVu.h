#pragma once

#include "afxcmn.h"

class CPositionOptionDeltasVu :  public CListCtrl  {
  DECLARE_DYNCREATE(CPositionOptionDeltasVu)
public:
  CPositionOptionDeltasVu(void);
  virtual ~CPositionOptionDeltasVu(void);
protected:
	DECLARE_MESSAGE_MAP()

  afx_msg int OnCreate( LPCREATESTRUCT lpCreateStruct );
  afx_msg void OnDestroy( );
  afx_msg void OnClose( );  // The default implementation calls DestroyWindow.
  afx_msg void OnCustomDraw( NMHDR* pNMHDR, LRESULT* pResult );



private:
};
