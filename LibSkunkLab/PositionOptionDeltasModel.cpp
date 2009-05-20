#include "StdAfx.h"
#include "PositionOptionDeltasModel.h"

CPositionOptionDeltasModel::CPositionOptionDeltasModel(void) {
}

CPositionOptionDeltasModel::~CPositionOptionDeltasModel(void) {
  for ( std::vector<CPositionOptionDeltasRow*>::iterator iter = m_rows.begin(); iter != m_rows.end(); ++iter ) {
    CPositionOptionDeltasRow* pRow = *iter;
    *iter = NULL;
    delete pRow;
  }
}
