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

#include "NodeTimeSeries.h"

namespace ou { // One Unified
namespace gp { // genetic programming

NodeTSTrade::NodeTSTrade(void): NodeTimeSeries<NodeTSTrade, ou::tf::CTrades>() {
  m_cntNodes = 0;
}

NodeTSTrade::~NodeTSTrade(void) {
}

double NodeTSTrade::EvaluateDouble( void ) {
  return TimeSeries()->Last()->Trade();
}

// =======================

NodeTSQuoteBid::NodeTSQuoteBid(void): NodeTimeSeries<NodeTSQuoteBid, ou::tf::CQuotes>() {
  m_cntNodes = 0;
}

NodeTSQuoteBid::~NodeTSQuoteBid(void) {
}

double NodeTSQuoteBid::EvaluateDouble( void ) {
  return TimeSeries()->Last()->Bid();
}

// =======================

NodeTSQuoteAsk::NodeTSQuoteAsk(void): NodeTimeSeries<NodeTSQuoteAsk, ou::tf::CQuotes>() {
  m_cntNodes = 0;
}

NodeTSQuoteAsk::~NodeTSQuoteAsk(void) {
}

double NodeTSQuoteAsk::EvaluateDouble( void ) {
  return TimeSeries()->Last()->Ask();
}

// =======================

NodeTSQuoteMid::NodeTSQuoteMid(void): NodeTimeSeries<NodeTSQuoteMid, ou::tf::CQuotes>() {
  m_cntNodes = 0;
}

NodeTSQuoteMid::~NodeTSQuoteMid(void) {
}

double NodeTSQuoteMid::EvaluateDouble( void ) {
  return TimeSeries()->Last()->Midpoint();
}

// =======================

NodeTSPrice::NodeTSPrice(void): NodeTimeSeries<NodeTSPrice, ou::tf::CPrices>() {
  m_cntNodes = 0;
}

NodeTSPrice::~NodeTSPrice(void) {
}

double NodeTSPrice::EvaluateDouble( void ) {
  return TimeSeries()->Last()->Price();
}

// =======================

} // namespace gp
} // namespace ou
