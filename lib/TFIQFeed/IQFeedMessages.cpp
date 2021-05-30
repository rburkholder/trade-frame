/************************************************************************
 * Copyright(c) 2009, One Unified. All rights reserved.                 *
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

#include <sstream>

#include "IQFeedMessages.h"

// News Message: This does appear to be an error in the documentation.
// "SNT:AAPL::1:20070901:;"
// Enter the date with a range, such as "20070901-20070910" to get the data you are looking for.

namespace ou { // One Unified
namespace tf { // TradeFrame

//**** IQFBaseMessage

// templated

//**** IQFSystemMessage

IQFSystemMessage::IQFSystemMessage( void )
: IQFBaseMessage<IQFSystemMessage>()
{
}

IQFSystemMessage::IQFSystemMessage( iterator_t& current, iterator_t& end )
: IQFBaseMessage<IQFSystemMessage>( current, end )
{
}

IQFSystemMessage::~IQFSystemMessage() {
}

//**** IQFErrorMessage

IQFErrorMessage::IQFErrorMessage( void )
: IQFBaseMessage<IQFErrorMessage>()
{
}

IQFErrorMessage::IQFErrorMessage( iterator_t& current, iterator_t& end )
: IQFBaseMessage<IQFErrorMessage>( current, end )
{
}

IQFErrorMessage::~IQFErrorMessage() {
}

//**** IQFNewsMessage

IQFNewsMessage::IQFNewsMessage( void )
: IQFBaseMessage<IQFNewsMessage>()
{
}

IQFNewsMessage::IQFNewsMessage( iterator_t& current, iterator_t& end )
: IQFBaseMessage<IQFNewsMessage>()
{
}

IQFNewsMessage::~IQFNewsMessage() {
}

//**** IQFFundamentalMessage
// resize the vector to accept with out resizing so often?

IQFFundamentalMessage::IQFFundamentalMessage( void )
: IQFBaseMessage<IQFFundamentalMessage>()
{
}

IQFFundamentalMessage::IQFFundamentalMessage( iterator_t& current, iterator_t& end )
: IQFBaseMessage<IQFFundamentalMessage>( current, end )
{
}

IQFFundamentalMessage::~IQFFundamentalMessage() {
}

//**** IQFUpdateMessage

IQFUpdateMessage::IQFUpdateMessage( void )
: IQFPricingMessage<IQFUpdateMessage>()
{
}

IQFUpdateMessage::IQFUpdateMessage( iterator_t& current, iterator_t& end )
: IQFPricingMessage<IQFUpdateMessage>( current, end )
{
}

IQFUpdateMessage::~IQFUpdateMessage() {
}

//**** IQFSummaryMessage

IQFSummaryMessage::IQFSummaryMessage( void )
: IQFPricingMessage<IQFSummaryMessage>()
{
}

IQFSummaryMessage::IQFSummaryMessage( iterator_t& current, iterator_t& end )
: IQFPricingMessage<IQFSummaryMessage>( current, end )
{
}

IQFSummaryMessage::~IQFSummaryMessage() {
}

//**** IQFTimeMessage

IQFTimeMessage::IQFTimeMessage( void )
: IQFBaseMessage<IQFTimeMessage>(),
  m_bMarketIsOpen( false ),
  m_timeMarketOpen( time_duration( 9, 30, 0 ) ), m_timeMarketClose( time_duration( 16, 0, 0 ) )
{
}

IQFTimeMessage::IQFTimeMessage( iterator_t& current, iterator_t& end )
: IQFBaseMessage<IQFTimeMessage>(),
  m_bMarketIsOpen( false ),
  m_timeMarketOpen( time_duration( 9, 30, 0 ) ), m_timeMarketClose( time_duration( 16, 0, 0 ) )
{
  Assign( current, end );
}

IQFTimeMessage::~IQFTimeMessage() {
}

void IQFTimeMessage::Assign(iterator_t &current, iterator_t &end) {
  IQFBaseMessage<IQFTimeMessage>::Assign( current, end );
  std::stringstream ss( Field( 2 ) );
  time_input_facet *input_facet;
  input_facet = new boost::posix_time::time_input_facet();  // input facet stuff needs to be with ss.imbue, can't be reused
  input_facet->format( "%Y%m%d %H:%M:%S" );
  ss.imbue( std::locale( ss.getloc(), input_facet ) );
  ss >> m_dt;
  m_bMarketIsOpen = ( ( m_dt.time_of_day() >= m_timeMarketOpen ) && ( m_dt.time_of_day() < m_timeMarketClose ) );
}

} // namespace tf
} // namespace ou
