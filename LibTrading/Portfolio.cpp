#include "StdAfx.h"
#include "Portfolio.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CPortfolio::CPortfolio( const std::string &sPortfolioName ) 
: m_sPortfolioName( sPortfolioName )
{
}

CPortfolio::~CPortfolio(void) {
}
