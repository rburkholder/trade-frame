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

#include <boost/fusion/container/vector.hpp>

#include "NodeBoolean.h"

namespace ou { // One Unified
namespace gp { // genetic programming

class NodeCompareGT: public Node {
public:
  NodeCompareGT( void );
  ~NodeCompareGT( void );
  void ToString( std::stringstream& ss ) const { ss << ">"; };
  bool EvaluateBoolean( void ) const;
protected:
private:
};

class NodeCompareGE: public Node {
public:
  NodeCompareGE( void );
  ~NodeCompareGE( void );
  void ToString( std::stringstream& ss ) const { ss << ">="; };
  bool EvaluateBoolean( void ) const;
protected:
private:
};

class NodeCompareLT: public Node {
public:
  NodeCompareLT( void );
  ~NodeCompareLT( void );
  void ToString( std::stringstream& ss ) const { ss << "<"; };
  bool EvaluateBoolean( void ) const;
protected:
private:
};

class NodeCompareLE: public Node {
public:
  NodeCompareLE( void );
  ~NodeCompareLE( void );
  void ToString( std::stringstream& ss ) const { ss << "<="; };
  bool EvaluateBoolean( void ) const;
protected:
private:
};

typedef boost::fusion::vector<NodeCompareGT, NodeCompareGE, NodeCompareLT, NodeCompareLE> NodeCompare_t;

} // namespace gp
} // namespace ou
