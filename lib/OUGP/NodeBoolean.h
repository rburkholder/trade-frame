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

#include "Node.h"

namespace ou { // One Unified
namespace gp { // genetic programming

class NodeBooleanFalse: public Node {
public:
  NodeBooleanFalse( void );
  ~NodeBooleanFalse( void );
  void ToString( std::stringstream& ss ) const { ss << "false"; };
  bool EvaluateBoolean( void ) const { return false; };
protected:
private:
};

class NodeBooleanTrue: public Node {
public:
  NodeBooleanTrue( void );
  ~NodeBooleanTrue( void );
  void ToString( std::stringstream& ss ) const { ss << "true"; };
  bool EvaluateBoolean( void ) const { return true; };
protected:
private:
};

class NodeBooleanNot: public Node {
public:
  NodeBooleanNot( void );
  ~NodeBooleanNot( void );
  void ToString( std::stringstream& ss ) const { ss << "!"; };
  bool EvaluateBoolean( void ) const;
protected:
private:
};

class NodeBooleanAnd: public Node {
public:
  NodeBooleanAnd( void );
  ~NodeBooleanAnd( void );
  void ToString( std::stringstream& ss ) const { ss << "&&"; };
  bool EvaluateBoolean( void ) const;
protected:
private:
};

class NodeBooleanOr: public Node {
public:
  NodeBooleanOr( void );
  ~NodeBooleanOr( void );
  void ToString( std::stringstream& ss ) const { ss << "||"; };
  bool EvaluateBoolean( void ) const;
protected:
private:
};

typedef boost::fusion::vector<NodeBooleanFalse, NodeBooleanTrue, NodeBooleanNot, NodeBooleanAnd, NodeBooleanOr> NodeBoolean_t;

} // namespace gp
} // namespace ou
