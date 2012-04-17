#include "StdAfx.h"
#include "TSVariance.h"

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace hf { // high frequency

TSVariance::TSVariance( CPrices& series, time_duration dt, unsigned int n, double p1, double p2 ) 
  : m_seriesSource( series ), m_dtTimeRange( dt ), m_n( n ), m_p1( p1 ), m_p2( p2 ),
    m_ma1( series, dt, n ), m_ma2( m_dummy, dt, n )
{
  assert( 0 < n );
  assert( 0.0 < p2 );
  series.OnAppend.Add( MakeDelegate( this, &TSVariance::HandleUpdate ) );
  m_ma1.OnAppend.Add( MakeDelegate( this, &TSVariance::HandleMA1Update ) );
  m_ma2.OnAppend.Add( MakeDelegate( this, &TSVariance::HandleMA2Update ) );
}

TSVariance::~TSVariance(void) {
  m_seriesSource.OnAppend.Remove( MakeDelegate( this, &TSVariance::HandleUpdate ) );
  m_ma1.OnAppend.Remove( MakeDelegate( this, &TSVariance::HandleMA1Update ) );
  m_ma2.OnAppend.Remove( MakeDelegate( this, &TSVariance::HandleMA2Update ) );
}

void TSVariance::HandleUpdate( const CPrice& price ) {
  m_z = price.Price();
}

void TSVariance::HandleMA1Update( const CPrice& price ) {
  double t = m_z - price.Price();
  if ( 1.0 == m_p1 ) {
    m_dummy.Append( CPrice( price.DateTime(), std::abs( t ) ) );
  }
  else {
    if ( 2.0 == m_p1 ) {
      double t = m_z - price.Price();
      m_dummy.Append( CPrice( price.DateTime(), t * t ) );
    }
    else {
      m_dummy.Append( CPrice( price.DateTime(), std::pow( std::abs( t ), m_p1 ) ) );
    }
  }
}

void TSVariance::HandleMA2Update( const CPrice& price ) {
  if ( 1.0 == m_p2 ) {
    Append( price );
  }
  else {
    if ( 2.0 == m_p2 ) {
      Append( CPrice( price.DateTime(), std::sqrt( price.Price() ) ) );
    }
    else {
      Append( CPrice( price.DateTime(), std::pow( price.Price(), 1.0 / m_p2 ) ) );
    }
  }
}

} // namespace hf
} // namespace tf
} // namespace ou
