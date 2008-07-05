#include "StdAfx.h"
#include "ChartInstrumentTree.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNAMIC(CChartInstrumentTree, CTreeCtrl)

CChartInstrumentTree::CChartInstrumentTree(void)
: CTreeCtrl()
{
}

CChartInstrumentTree::~CChartInstrumentTree(void) {
}

BEGIN_MESSAGE_MAP(CChartInstrumentTree, CTreeCtrl)
END_MESSAGE_MAP()
