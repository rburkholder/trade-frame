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

#include "NodeBoolean.h"

namespace ou { // One Unified
namespace gp { // genetic programming

class NodeCompareGT: public Node<NodeCompareGT> {
  friend Node<NodeCompareGT>;
public:
  NodeCompareGT( void );
  ~NodeCompareGT( void );
  void ToString( std::stringstream& ss ) const { ss << ">"; };
protected:
private:
  bool EvaluateBooleanImpl( void ) const;
};

class NodeCompareGE: public Node<NodeCompareGE> {
  friend Node<NodeCompareGE>;
public:
  NodeCompareGE( void );
  ~NodeCompareGE( void );
  void ToString( std::stringstream& ss ) const { ss << ">="; };
protected:
private:
  bool EvaluateBooleanImpl( void ) const;
};

class NodeCompareLT: public Node<NodeCompareLT> {
  friend Node<NodeCompareLT>;
public:
  NodeCompareLT( void );
  ~NodeCompareLT( void );
  void ToString( std::stringstream& ss ) const { ss << "<"; };
protected:
private:
  bool EvaluateBooleanImpl( void ) const;
};

class NodeCompareLE: public Node<NodeCompareLE> {
  friend Node<NodeCompareLE>;
public:
  NodeCompareLE( void );
  ~NodeCompareLE( void );
  void ToString( std::stringstream& ss ) const { ss << "<="; };
protected:
private:
  bool EvaluateBooleanImpl( void ) const;
};



} // namespace gp
} // namespace ou
