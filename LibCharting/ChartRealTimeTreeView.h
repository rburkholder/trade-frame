#pragma once
#include "GUIFrameBase.h"
#include "ChartInstrumentTree.h"
#include "ChartDataView.h"

// run as a singleton, actually, shouldn't need to worry about his, as container should be a singleton, at least it controls instantiations

class CChartRealTimeTreeView : public CGUIFrameBase {
  DECLARE_DYNAMIC(CChartRealTimeTreeView)
public:
  virtual ~CChartRealTimeTreeView(void);
  static CChartRealTimeTreeView *Register( void );
  static void Deregister( void );
  void Add( const std::string &sStrategy, const std::string &sName, CChartDataView *pDataView ) { // caller handles destruction
    m_Tree.Add( sStrategy, sName, pDataView );
  };
  void Remove( const std::string &sStrategy, const std::string &sName ) {
    m_Tree.Remove( sStrategy, sName );
  };
protected:
	afx_msg void OnDestroy();
  afx_msg void OnSize(UINT nType, int cx, int cy);
  CChartInstrumentTree m_Tree;
  afx_msg void OnTvnSelChangedTree(NMHDR *pNMHDR, LRESULT *pResult);
private:
  explicit CChartRealTimeTreeView(void);  // here for singleton's sake
  DECLARE_MESSAGE_MAP()
  static CChartRealTimeTreeView *m_pTreeView;
  static size_t m_cntConstruction;
};
