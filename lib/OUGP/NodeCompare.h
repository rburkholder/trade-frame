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

template<class T>
class NodeCompare: public NodeProxy<T> {
public:
  NodeCompare( void ): NodeProxy<T>( NodeType::Bool, NodeType::Double ) {};
  ~NodeCompare( void ) {};
protected:
private:
};

class NodeCompareGT: public NodeCompare<NodeCompareGT> {
public:
  NodeCompareGT( void );
  ~NodeCompareGT( void );
  void ToString( std::stringstream& ss ) const { ss << ">"; };
  bool EvaluateBoolean( void );
protected:
private:
};

class NodeCompareGE: public NodeCompare<NodeCompareGE> {
public:
  NodeCompareGE( void );
  ~NodeCompareGE( void );
  void ToString( std::stringstream& ss ) const { ss << ">="; };
  bool EvaluateBoolean( void );
protected:
private:
};

class NodeCompareLT: public NodeCompare<NodeCompareLT> {
public:
  NodeCompareLT( void );
  ~NodeCompareLT( void );
  void ToString( std::stringstream& ss ) const { ss << "<"; };
  bool EvaluateBoolean( void );
protected:
private:
};

class NodeCompareLE: public NodeCompare<NodeCompareLE> {
public:
  NodeCompareLE( void );
  ~NodeCompareLE( void );
  void ToString( std::stringstream& ss ) const { ss << "<="; };
  bool EvaluateBoolean( void );
protected:
private:
};

typedef boost::fusion::vector<NodeCompareGT, NodeCompareGE, NodeCompareLT, NodeCompareLE> NodeCompare_t;

} // namespace gp
} // namespace ou
