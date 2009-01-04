#include "StdAfx.h"
#include "PortfolioRecord.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CPortfolioRecord::CPortfolioRecord(void) 
: m_pInstrument( NULL ), m_nPosition( 0 ), m_dblPrice( 0 ), m_dblAverageCost( 0 )
{
}

CPortfolioRecord::~CPortfolioRecord(void) {
}
