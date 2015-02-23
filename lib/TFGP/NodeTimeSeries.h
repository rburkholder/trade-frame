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

#include <TFTimeSeries/TimeSeries.h>

#include <OUGP/Node.h>

#include "TimeSeriesForNode.h"
#include "TimeSeriesRegistration.h"

namespace ou { // One Unified
namespace gp { // genetic programming

template<typename N, typename TS>  // Specific Node, TimeSeries
class NodeTimeSeries: public NodeDouble<N>, public TimeSeriesForNode<TS> {
public:
  NodeTimeSeries( void );
  NodeTimeSeries( const NodeTimeSeries& rhs );
  ~NodeTimeSeries( void );
  virtual void PreProcess( void ) {
    TimeSeriesRegistration<TS>::SetTimeSeries( &this->m_pTimeSeries, this->m_ixTimeSeries );
  }
protected:
private:
};

template<typename N, typename TS>
NodeTimeSeries<N,TS>::NodeTimeSeries( void ): NodeDouble<N>(), TimeSeriesForNode<TS>() {
}

template<typename N, typename TS>
NodeTimeSeries<N,TS>::NodeTimeSeries( const NodeTimeSeries& rhs )
  : NodeDouble<N>( rhs ), TimeSeriesForNode<TS>( rhs )
{
}

template<typename N, typename TS>
NodeTimeSeries<N,TS>::~NodeTimeSeries( void ) {
}

// =======================

class NodeTSTrade: public NodeTimeSeries<NodeTSTrade, ou::tf::Trades> {
public:
  NodeTSTrade(void);
  ~NodeTSTrade(void);
  void ToString( std::stringstream& ss ) const { ss << m_pTimeSeries->GetName() << ".price()"; };
  double EvaluateDouble( void );
protected:
private:
};

// =======================

class NodeTSQuoteBid: public NodeTimeSeries<NodeTSQuoteBid, ou::tf::Quotes> {
public:
  NodeTSQuoteBid(void);
  ~NodeTSQuoteBid(void);
  void ToString( std::stringstream& ss ) const { ss << m_pTimeSeries->GetName() << ".bid()"; };
  double EvaluateDouble( void );
protected:
private:
};

class NodeTSQuoteAsk: public NodeTimeSeries<NodeTSQuoteAsk, ou::tf::Quotes> {
public:
  NodeTSQuoteAsk(void);
  ~NodeTSQuoteAsk(void);
  void ToString( std::stringstream& ss ) const { ss << m_pTimeSeries->GetName() << ".ask()"; };
  double EvaluateDouble( void );
protected:
private:
};

class NodeTSQuoteMid: public NodeTimeSeries<NodeTSQuoteMid, ou::tf::Quotes> {
public:
  NodeTSQuoteMid(void);
  ~NodeTSQuoteMid(void);
  void ToString( std::stringstream& ss ) const { ss << m_pTimeSeries->GetName() << ".mid()"; };
  double EvaluateDouble( void );
protected:
private:
};

// =======================

class NodeTSPrice: public NodeTimeSeries<NodeTSPrice, ou::tf::Prices> {
public:
  NodeTSPrice(void);
  ~NodeTSPrice(void);
  void ToString( std::stringstream& ss ) const { ss << m_pTimeSeries->GetName() << ".value()"; };
  double EvaluateDouble( void );
protected:
private:
};

// =======================

template<typename Node, size_t ixTimeSeries>
class IndexedNode: public Node {
public:
  IndexedNode( void ) {
    this->m_ixTimeSeries = ixTimeSeries;
  };
  ~IndexedNode( void ) {};
  
protected:
private:
};

} // namespace gp
} // namespace ou
