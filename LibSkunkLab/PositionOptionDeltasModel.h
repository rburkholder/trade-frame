#pragma once

#include <vector>

#include "PositionOptionDeltasRow.h"

class CPositionOptionDeltasModel {
public:
  CPositionOptionDeltasModel(void);
  ~CPositionOptionDeltasModel(void);
protected:
private:
  std::vector<CPositionOptionDeltasRow*> m_rows;
};
