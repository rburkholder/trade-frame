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

class RootNode: public Node<RootNode> {
  friend Node<RootNode>;
public:
  RootNode(void);
  ~RootNode(void);
  void ToString( std::stringstream& ss ) const { ss << "root="; };
protected:
private:
  bool EvaluateBooleanImpl( void ) const;
};

class NodeBooleanFalse: public Node<NodeBooleanFalse> {
  friend Node<NodeBooleanFalse>;
public:
  NodeBooleanFalse( void );
  ~NodeBooleanFalse( void );
  void ToString( std::stringstream& ss ) const { ss << "false"; };
protected:
private:
  bool EvaluateBooleanImpl( void ) const { return false; };
};

class NodeBooleanTrue: public Node<NodeBooleanTrue> {
  friend Node<NodeBooleanTrue>;
public:
  NodeBooleanTrue( void );
  ~NodeBooleanTrue( void );
  void ToString( std::stringstream& ss ) const { ss << "true"; };
protected:
private:
  bool EvaluateBooleanImpl( void ) const { return true; };
};

class NodeBooleanNot: public Node<NodeBooleanNot> {
  friend Node<NodeBooleanNot>;
public:
  NodeBooleanNot( void );
  ~NodeBooleanNot( void );
  void ToString( std::stringstream& ss ) const { ss << "!"; };
protected:
private:
  bool EvaluateBooleanImpl( void ) const;
};

class NodeBooleanAnd: public Node<NodeBooleanAnd> {
  friend Node<NodeBooleanAnd>;
public:
  NodeBooleanAnd( void );
  ~NodeBooleanAnd( void );
  void ToString( std::stringstream& ss ) const { ss << "&&"; };
protected:
private:
  bool EvaluateBooleanImpl( void ) const;
};

class NodeBooleanOr: public Node<NodeBooleanOr> {
  friend Node<NodeBooleanOr>;
public:
  NodeBooleanOr( void );
  ~NodeBooleanOr( void );
  void ToString( std::stringstream& ss ) const { ss << "||"; };
protected:
private:
  bool EvaluateBooleanImpl( void ) const;
};

} // namespace gp
} // namespace ou
