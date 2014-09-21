/************************************************************************
 * Copyright(c) 2014, One Unified. All rights reserved.                 *
 * email: info@oneunified.net                                           *
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

#include "PanelOptionDetails.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

class PanelOptionDetails_impl {
public:
  PanelOptionDetails_impl( PanelOptionDetails& );
  ~PanelOptionDetails_impl(void);
protected:

private:

  PanelOptionDetails& m_details;

};

} // namespace tf
} // namespace ou