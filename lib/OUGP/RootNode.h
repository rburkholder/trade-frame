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

#include <vector>

#include <boost/shared_ptr.hpp>

#include "Node.h"

namespace ou { // One Unified
namespace gp { // genetic programming

class RootNode: public NodeProxy<RootNode> {
public:

  typedef std::vector<Node*> vpNode_t;

  RootNode(void);
  ~RootNode(void);

  void ToString( std::stringstream& ss ) const { ss << "root="; };
  bool EvaluateBoolean( void );

  void PopulateCandidates( void );  // ** todo:  calculate maximum depth?

protected:
private:

  vpNode_t m_vAllCandidates;
  vpNode_t m_vBooleanCandidates;
  vpNode_t m_vDoubleCandidates;
  vpNode_t m_vTerminalCandidates;
  vpNode_t m_vFunctionCandidates;

  void AddCandidateNode( Node* );

};


} // namespace gp
} // namespace ou
