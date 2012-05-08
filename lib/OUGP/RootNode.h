/************************************************************************
 * Copyright(c) 2012, One Unified. All rights reserved.                 *
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

#include "Node.h"

namespace ou { // One Unified
namespace gp { // genetic programming

class RootNode: public Node {
public:
  RootNode(void);
  ~RootNode(void);
  void ToString( std::stringstream& ss ) const { ss << "root="; };
  bool EvaluateBoolean( void );
protected:
private:
};


} // namespace gp
} // namespace ou
