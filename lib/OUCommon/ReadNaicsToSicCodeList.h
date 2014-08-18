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

// started 2014/08/17

#include "ReadCodeListCommon.h"

namespace ou { // One Unified

class ReadNaicsToSicCodeList: public ReadCodeListCommon {
public:

  ReadNaicsToSicCodeList( const std::string& sFileName );
  ~ReadNaicsToSicCodeList(void);

protected:
private:
};

} // namespace ou