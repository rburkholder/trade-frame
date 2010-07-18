/************************************************************************
 * Copyright(c) 2009, One Unified. All rights reserved.                 *
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

#include <string>

#include "ManagerBase.h"
#include "Execution.h"

class CExecutionManager: public ManagerBase<CExecutionManager, CExecution::ExecutionId_t, CExecution> {
public:
  CExecutionManager(void);
  ~CExecutionManager(void);
protected:
private:
};
