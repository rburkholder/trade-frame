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

#include "node.h"

namespace ou { // One Unified
namespace gp { // genetic programming

class NodeDoubleAdd: public Node<NodeDoubleAdd> {
  friend Node<NodeDoubleAdd>;
public:
  NodeDoubleAdd( void );
  ~NodeDoubleAdd( void );
  void ToString( std::stringstream& ss ) const { ss << "+"; };
protected:
private:
  double EvaluateDoubleImpl( void ) const;
};

class NodeDoubleSub: public Node<NodeDoubleSub> {
  friend Node<NodeDoubleSub>;
public:
  NodeDoubleSub( void );
  ~NodeDoubleSub( void );
  void ToString( std::stringstream& ss ) const { ss << "-"; };
protected:
private:
  double EvaluateDoubleImpl( void ) const;
};

class NodeDoubleMlt: public Node<NodeDoubleMlt> {
  friend Node<NodeDoubleMlt>;
public:
  NodeDoubleMlt( void );
  ~NodeDoubleMlt( void );
  void ToString( std::stringstream& ss ) const { ss << "*"; };
protected:
private:
  double EvaluateDoubleImpl( void ) const;
};

class NodeDoubleDvd: public Node<NodeDoubleDvd> {
  friend Node<NodeDoubleDvd>;
public:
  NodeDoubleDvd( void );
  ~NodeDoubleDvd( void );
  void ToString( std::stringstream& ss ) const { ss << "/"; };
protected:
private:
  double EvaluateDoubleImpl( void ) const;
};


} // namespace gp
} // namespace ou
