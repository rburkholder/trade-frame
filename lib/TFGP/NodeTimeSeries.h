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

#include <TFTimeSeries/TimeSeries.h>

#include <OUGP/Node.h>

#include "TimeSeriesForNode.h"

namespace ou { // One Unified
namespace gp { // genetic programming

template<typename N, typename TS>  // Specific Node, TimeSeries
class NodeTimeSeries: public NodeDouble<N>, public TimeSeriesForNode<TS> {
public:
  NodeTimeSeries( void );
  ~NodeTimeSeries( void );
protected:
private:
};

template<typename N, typename TS>
NodeTimeSeries<N,TS>::NodeTimeSeries( void ): NodeDouble<N>(), TimeSeriesForNode<TS>() {
}

template<typename N, typename TS>
NodeTimeSeries<N,TS>::~NodeTimeSeries( void ) {
}

// =======================

class NodeTSTrade: public NodeTimeSeries<NodeTSTrade, ou::tf::CTrades> {
public:
  NodeTSTrade(void);
  ~NodeTSTrade(void);
  void ToString( std::stringstream& ss ) const { ss << "trade()"; };
  double EvaluateDouble( void );
protected:
private:
};

// =======================

class NodeTSQuoteBid: public NodeTimeSeries<NodeTSQuoteBid, ou::tf::CQuotes> {
public:
  NodeTSQuoteBid(void);
  ~NodeTSQuoteBid(void);
  void ToString( std::stringstream& ss ) const { ss << "bid()"; };
  double EvaluateDouble( void );
protected:
private:
};

class NodeTSQuoteAsk: public NodeTimeSeries<NodeTSQuoteAsk, ou::tf::CQuotes> {
public:
  NodeTSQuoteAsk(void);
  ~NodeTSQuoteAsk(void);
  void ToString( std::stringstream& ss ) const { ss << "ask()"; };
  double EvaluateDouble( void );
protected:
private:
};

class NodeTSQuoteMid: public NodeTimeSeries<NodeTSQuoteMid, ou::tf::CQuotes> {
public:
  NodeTSQuoteMid(void);
  ~NodeTSQuoteMid(void);
  void ToString( std::stringstream& ss ) const { ss << "mid()"; };
  double EvaluateDouble( void );
protected:
private:
};

// =======================

class NodeTSPrice: public NodeTimeSeries<NodeTSPrice, ou::tf::CPrices> {
public:
  NodeTSPrice(void);
  ~NodeTSPrice(void);
  void ToString( std::stringstream& ss ) const { ss << "price()"; };
  double EvaluateDouble( void );
protected:
private:
};

// =======================

typedef boost::fusion::vector<NodeTSTrade, NodeTSQuoteBid, NodeTSQuoteAsk, NodeTSQuoteMid, NodeTSPrice> NodeTypesTimeSeries_t;

} // namespace gp
} // namespace ou
