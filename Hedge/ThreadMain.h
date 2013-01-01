/************************************************************************
 * Copyright(c) 2011, One Unified. All rights reserved.                 *
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

#include "DB.h"

#include "Process.h"

class ThreadMain {
public:
  ThreadMain(enumMode eMode);
  ~ThreadMain(void);
protected:
private:
  enumMode m_eMode;
  DB m_db;
  Process* m_pProcess;
};

