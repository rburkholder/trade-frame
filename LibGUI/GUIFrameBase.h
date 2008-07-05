#pragma once
//#include "afxwin.h"

#include <string>

class CGUIFrameBase : public CFrameWnd {
    DECLARE_DYNAMIC(CGUIFrameBase)
public:
  CGUIFrameBase(CWnd* pParent =NULL);
  ~CGUIFrameBase(void);
  void SetTitleBarText( const std::string &sTitle );
  void SetPosition( long left, long top, long right, long bottom ); 
  virtual BOOL Create( void );
protected:
  std::string m_sDialogTitle;
  CRect m_position;
  CWnd *m_pParent;
	afx_msg void OnDestroy();
  afx_msg void OnSize(UINT nType, int cx, int cy);
private:
	DECLARE_MESSAGE_MAP()
};
