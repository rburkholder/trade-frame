#include "StdAfx.h"
#include "TSVariance.h"

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace hf { // high frequency

TSVariance::TSVariance( Prices& series, time_duration dt, unsigned int n, double p1, double p2 ) 
  : m_seriesSource( series ), m_dtTimeRange( dt ), m_n( n ), m_p1( p1 ), m_p2( p2 ),
    m_ma2( m_dummy, dt, n )
{
  assert( 0 < m_n );
  assert( 0.0 < m_p2 );
  series.OnAppend.Add( MakeDelegate( this, &TSVariance::HandleUpdate ) );
  m_pma1 = new TSMA( series, dt, n );
  m_pma1->OnAppend.Add( MakeDelegate( this, &TSVariance::HandleMA1Update ) );
  m_dummy.OnAppend.Add( MakeDelegate( this, &TSVariance::HandleMA2Update ) );
}

TSVariance::~TSVariance(void) {
  m_seriesSource.OnAppend.Remove( MakeDelegate( this, &TSVariance::HandleUpdate ) );
  m_pma1->OnAppend.Remove( MakeDelegate( this, &TSVariance::HandleMA1Update ) );
  m_dummy.OnAppend.Remove( MakeDelegate( this, &TSVariance::HandleMA2Update ) );
  delete m_pma1;
}

void TSVariance::HandleUpdate( const Price& price ) {
  m_z = price.Value();
}

void TSVariance::HandleMA1Update( const Price& price ) {
  double t = m_z - price.Value();
  if ( 1.0 == m_p1 ) {
    m_dummy.Append( Price( price.DateTime(), std::abs( t ) ) );
  }
  else {
    if ( 2.0 == m_p1 ) {
      m_dummy.Append( Price( price.DateTime(), t * t ) );
    }
    else {
      m_dummy.Append( Price( price.DateTime(), std::pow( std::abs( t ), m_p1 ) ) );
    }
  }
}

void TSVariance::HandleMA2Update( const Price& price ) {
  if ( 1.0 == m_p2 ) {
    Append( price );
  }
  else {
    if ( 2.0 == m_p2 ) {
      Append( Price( price.DateTime(), std::sqrt( price.Value() ) ) );
    }
    else {
      Append( Price( price.DateTime(), std::pow( price.Value(), 1.0 / m_p2 ) ) );
    }
  }
}

} // namespace hf
} // namespace tf
} // namespace ou
