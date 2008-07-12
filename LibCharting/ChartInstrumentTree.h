#pragma once

#include <string>

#include "ChartDataView.h"
#include "Delegate.h"

// CChartInstrumentTree is used the control for showing/selecting charts in the ChartViewPort
// CChartRealTimeTreeView is the window containing this control

class CChartInstrumentTree : public CTreeCtrl {
  DECLARE_DYNAMIC(CChartInstrumentTree)
public:
  CChartInstrumentTree(void);
  virtual ~CChartInstrumentTree(void);
  void Add( const std::string &sStrategy, const std::string &sName, CChartDataView *pDataView );
  void Remove( const std::string &sStrategy, const std::string &sName );
  //void Remove( const std::string &sStrategy );
  Delegate<CChartDataView *> OnClick;
  Delegate<CChartDataView *> OnRemove;
  void ProcessSelectionChanged( LPNMTREEVIEW pNMTreeView );
protected:
private:
  DECLARE_MESSAGE_MAP()

};
