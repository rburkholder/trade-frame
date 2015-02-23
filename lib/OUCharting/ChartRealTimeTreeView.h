/************************************************************************
 * Copyright(c) 2013, One Unified. All rights reserved.                 *
 * email: info@oneunified.net                                           *
 *                                                                      *
 * This file is provided as is WITHOUT ANY WARRANTY                     *
 *  without even the implied warranty of                                *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.                *
 *                                                                      *
 * This software may not be used nor distributed without proper license *
 * agreement.                                                           *
 *                                                                      *
 * See the file LICENSE.txt for redistribution information.             *
 ************************************************************************/

#pragma once
#include "GUIFrameBase.h"
#include "ChartInstrumentTree.h"
#include "ChartDataView.h"

class CChartRealTimeTreeView : public CGUIFrameBase {
  DECLARE_DYNAMIC(CChartRealTimeTreeView)
  friend class CChartingContainer;
public:
  CChartRealTimeTreeView(void); 
  virtual ~CChartRealTimeTreeView(void);
  //static CChartRealTimeTreeView *Register( void );
  //static void Deregister( void );
  void Add( const std::string &sStrategy, const std::string &sName, CChartDataView *pDataView ) { // caller handles destruction
    m_Tree.Add( sStrategy, sName, pDataView );
  };
  void Remove( const std::string &sStrategy, const std::string &sName ) {
    m_Tree.Remove( sStrategy, sName );
  };
protected:
  afx_msg int OnCreate( LPCREATESTRUCT lpCreateStruct );
	afx_msg void OnDestroy();
  afx_msg void OnSize(UINT nType, int cx, int cy);
  CChartInstrumentTree m_Tree;
  afx_msg void OnTvnSelChangedTree(NMHDR *pNMHDR, LRESULT *pResult);
private:
  DECLARE_MESSAGE_MAP()
  //static CChartRealTimeTreeView *m_pTreeView;
  //static size_t m_cntConstruction;
};
