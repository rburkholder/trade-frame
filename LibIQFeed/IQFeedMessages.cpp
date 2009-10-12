/************************************************************************
 * Copyright(c) 2009, One Unified. All rights reserved.                 *
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

#include "StdAfx.h"
#include "IQFeedMessages.h"

#include <iostream>
#include <sstream>

#include <boost/assert.hpp>

// News Message: This does appear to be an error in the documentation. 
// "SNT:AAPL::1:20070901:;" 
// Enter the date with a range, such as "20070901-20070910" to get the data you are looking for.

//**** CIQFBaseMessage

//**** CIQFSystemMessage

CIQFSystemMessage::CIQFSystemMessage( iterator_t& current, iterator_t& end ) 
: CIQFBaseMessage<CIQFSystemMessage>( current, end )
{
}

CIQFSystemMessage::~CIQFSystemMessage() {
}

//**** CIQFNewsMessage

CIQFNewsMessage::CIQFNewsMessage( iterator_t& current, iterator_t& end ) 
: CIQFBaseMessage<CIQFNewsMessage>( current, end )
{
  m_sDistributor = Field( NDistributor );
  m_sStoryId = Field( NStoryId );
  m_sSymbolList = Field( NSymbolList );
  m_sDateTime = Field( NDateTime );
  fielddelimiter_t headline = m_vFieldDelimiters[ NHeadline ];
  m_sHeadline.assign( headline.first, end );
}

CIQFNewsMessage::~CIQFNewsMessage() {
}

//**** CIQFFundamentalMessage
// resize the vector to accept with out resizing so often?

CIQFFundamentalMessage::CIQFFundamentalMessage( iterator_t& current, iterator_t& end ) 
: CIQFBaseMessage<CIQFFundamentalMessage>( current, end )
{
}

CIQFFundamentalMessage::~CIQFFundamentalMessage() {
}

//**** CIQFUpdateMessage

CIQFUpdateMessage::CIQFUpdateMessage( iterator_t& current, iterator_t& end ) 
: CIQFPricingMessage<CIQFUpdateMessage>( current, end )
{
}

CIQFUpdateMessage::~CIQFUpdateMessage() {
}

//**** CIQFSummaryMessage

CIQFSummaryMessage::CIQFSummaryMessage( iterator_t& current, iterator_t& end ) 
: CIQFPricingMessage<CIQFSummaryMessage>( current, end )
{
}

CIQFSummaryMessage::~CIQFSummaryMessage() {
}

//**** CIQFTimeMessage

CIQFTimeMessage::CIQFTimeMessage( iterator_t& current, iterator_t& end ) 
: CIQFBaseMessage<CIQFTimeMessage>( current, end )
{
  m_bMarketIsOpen = false;
  m_timeMarketOpen = time_duration( 9, 30, 0 );
  m_timeMarketClose = time_duration( 16, 0, 0 );

  std::stringstream ss( Field( 2 ) );
  time_input_facet *input_facet;
  input_facet = new boost::posix_time::time_input_facet();  // input facet stuff needs to be with ss.imbue, can't be reused
  input_facet->format( "%Y%m%d %H:%M:%S" );
  ss.imbue( std::locale( ss.getloc(), input_facet ) );
  ss >> m_dt;
  m_bMarketIsOpen = ( ( m_dt.time_of_day() >= m_timeMarketOpen ) && ( m_dt.time_of_day() < m_timeMarketClose ) );
}

CIQFTimeMessage::~CIQFTimeMessage() {
}

