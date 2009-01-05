#pragma once

#include <string>
#include "ManagerBase.h"
#include "Execution.h"

class CExecutionManager: public ManagerBase<CExecutionManager, CExecution::ExecutionId_t, CExecution> {
public:
  CExecutionManager(void);
  virtual ~CExecutionManager(void);
protected:
private:
};
