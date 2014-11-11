#pragma once

#include "GUIFrameBase.h"

#include "GeneratePeriodicRefresh.h"

#include "ProviderInterface.h"
#include "Instrument.h"

#include "PositionOptionDeltasVu.h"
#include "PositionOptionDeltasMacros.h"
#include "PositionOptionDeltasControl.h"
#include "PositionOptionDeltasModel.h"


class CPositionOptionDeltasWnd: public CGUIFrameBase {
  DECLARE_DYNAMIC(CPositionOptionDeltasWnd)
public:
  CPositionOptionDeltasWnd( CWnd* pParent );
  virtual ~CPositionOptionDeltasWnd(void);
  virtual BOOL Create( void );
  void Add( CProviderInterface *pDataProvider, CInstrument::pInstrument_t pInstrument ) { 
    m_mdlDeltas.Add( pDataProvider, pInstrument ); 
  };
protected:
	DECLARE_MESSAGE_MAP()

  // model - view - controller
  CPositionOptionDeltasModel m_mdlDeltas;
  CPositionOptionDeltasVu m_vuDeltas;
  CPositionOptionDeltasControl m_ctlDeltas;

  enum enumColHdrDeltasCol {
    BOOST_PP_REPEAT( BOOST_PP_ARRAY_SIZE( COLHDR_DELTAS_ARRAY ), COLHDR_DELTAS_EXTRACT_ENUM_LIST, ~ )
  };

private:
  bool m_bDialogReady;

  afx_msg int OnCreate( LPCREATESTRUCT lpCreateStruct );
	afx_msg void OnDestroy();
  afx_msg void OnSize( UINT, int, int );
  afx_msg void OnMove( int x, int y );

  CGeneratePeriodicRefresh m_refresh;
  //void HandlePeriodicRefresh( CGeneratePeriodicRefresh* );


};

