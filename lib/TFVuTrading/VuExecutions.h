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

#include "VuBase.h"

#include "ModelExecution.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

class VuExecutions: public VuBase {
public:
  VuExecutions( ModelExecution* );
  VuExecutions( ModelExecution*, wxWindow *parent, wxWindowID id, 
    const wxPoint &pos=wxDefaultPosition, const wxSize &size=wxDefaultSize, 
    long style=0, const wxValidator &validator=wxDefaultValidator);
  ~VuExecutions(void);
protected:
private:

  typedef ModelExecution dvmdlExecutions_t;

  wxObjectDataPtr<dvmdlExecutions_t> m_pdvmdlExecutions;

  void Construct( void );

};

} // namespace tf
} // namespace ou
