#pragma once

#include "GUIFrameBase.h"

#include "PositionOptionDeltasVu.h"
#include "PositionOptionDeltasMacros.h"

class CPositionOptionDeltasWnd: public CGUIFrameBase {
  DECLARE_DYNAMIC(CPositionOptionDeltasWnd)
public:
  CPositionOptionDeltasWnd(CWnd* pParent =NULL);
  virtual ~CPositionOptionDeltasWnd(void);
  virtual BOOL Create( void );
protected:
	DECLARE_MESSAGE_MAP()

  CPositionOptionDeltasVu m_vuDeltas;

  enum enumColHdrDeltasCol {
    BOOST_PP_REPEAT( BOOST_PP_ARRAY_SIZE( COLHDR_DELTAS_ARRAY ), COLHDR_DELTAS_EXTRACT_ENUM_LIST, ~ )
  };

private:
  bool m_bDialogReady;

  afx_msg int OnCreate( LPCREATESTRUCT lpCreateStruct );
	afx_msg void OnDestroy();
  afx_msg void OnSize( UINT, int, int );
  afx_msg void OnMove( int x, int y );

};
