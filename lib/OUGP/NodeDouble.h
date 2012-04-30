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

#include "node.h"

namespace ou { // One Unified
namespace gp { // genetic programming

class NodeDoubleAdd: public Node {
public:
  NodeDoubleAdd( void );
  ~NodeDoubleAdd( void );
  void ToString( std::stringstream& ss ) const { ss << "+"; };
  double EvaluateDouble( void ) const;
protected:
private:
};

class NodeDoubleSub: public Node {
public:
  NodeDoubleSub( void );
  ~NodeDoubleSub( void );
  void ToString( std::stringstream& ss ) const { ss << "-"; };
  double EvaluateDouble( void ) const;
protected:
private:
};

class NodeDoubleMlt: public Node {
public:
  NodeDoubleMlt( void );
  ~NodeDoubleMlt( void );
  void ToString( std::stringstream& ss ) const { ss << "*"; };
  double EvaluateDouble( void ) const;
protected:
private:
};

class NodeDoubleDvd: public Node {
public:
  NodeDoubleDvd( void );
  ~NodeDoubleDvd( void );
  void ToString( std::stringstream& ss ) const { ss << "/"; };
  double EvaluateDouble( void ) const;
protected:
private:
};

typedef boost::fusion::vector<NodeDoubleAdd, NodeDoubleSub, NodeDoubleMlt, NodeDoubleDvd> NodeDouble_t;


} // namespace gp
} // namespace ou
