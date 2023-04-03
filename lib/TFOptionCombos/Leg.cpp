/************************************************************************
 * Copyright(c) 2019, One Unified. All rights reserved.                 *
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

/*
 * File:    Leg.cpp
 * Author:  raymond@burkholder.net
 * Project: TFOptionCombos
 * Created on May 25, 2019, 4:46 PM
 */

#include <boost/log/trivial.hpp>

#include "Leg.h"

namespace ou {
namespace tf {

Leg::Leg()
: m_bOption( false )
{
}

Leg::Leg( pPosition_t pPosition ) // implies candidate will not be used
: m_bOption( false )
{
  SetPosition( pPosition );
}

Leg::Leg( Leg&& rhs )
: m_pPosition( std::move( rhs.m_pPosition ) )
, m_bOption( rhs.m_bOption )
, m_legNote( std::move( rhs.m_legNote ) )
{
  assert( !rhs.m_pChartDataView );
  //m_pChartDataView = rhs.m_pChartDataView;
  //rhs.DelChartData();

  //m_ceProfitLoss = std::move( rhs.m_ceProfitLoss );
  //m_ceImpliedVolatility( std::move( rhs.m_ceImpliedVolatility ) )
  //m_ceDelta( std::move( rhs.m_ceDelta ) )
  //m_ceGamma( std::move( rhs.m_ceGamma ) )
  //m_ceVega( std::move( rhs.m_ceVega ) )
  //m_ceTheta( std::move( rhs.m_ceTheta ) )
  //rhs.DelChartData(); // deletes rhs.m_pChartDataView
  //m_pChartDataView = std::move( rhs.m_pChartDataView );
}

Leg& Leg::operator=( Leg&& rhs ) {
  if ( this != &rhs ) {
    m_pPosition = std::move( rhs.m_pPosition );
    m_legNote = std::move( rhs.m_legNote );
    m_bOption = rhs.m_bOption;
    assert( !rhs.m_pChartDataView );
    //rhs.DelChartData();
    //m_pChartDataView = std::move( rhs.m_pChartDataView );
  }
  return *this;
}

Leg::~Leg() {
  DelChartData();
  if ( m_pPosition ) {
    //BOOST_LOG_TRIVIAL(info) << "Leg destruction: " << m_pPosition->GetInstrument()->GetInstrumentName();
    m_pPosition.reset();
  }
  else {
    //BOOST_LOG_TRIVIAL(info) << "Leg destruction: unknown";
  }
}

const ou::tf::option::LegNote::values_t& Leg::SetPosition( pPosition_t pPosition ) {

  if ( m_pPosition ) {
    BOOST_LOG_TRIVIAL(info)
      << "Leg::SetPosition over-write position "
      << m_pPosition->GetInstrument()->GetInstrumentName()
      << " with "
      << pPosition->GetInstrument()->GetInstrumentName()
      ;

    DelChartData();
    m_pPosition.reset();
    m_bOption = false;
  }

  BOOST_LOG_TRIVIAL(info) << "Leg::SetPosition: " << pPosition->GetInstrument()->GetInstrumentName();

  m_pPosition = pPosition;
  m_legNote.Decode( m_pPosition->Notes() );

  ou::tf::Watch::pWatch_t pWatch = m_pPosition->GetWatch();
  // NOTE: this may generate error with non-option!
  ou::tf::option::Option::pOption_t pOption = std::dynamic_pointer_cast<ou::tf::option::Option>( pWatch );

  if ( pOption ) {
    m_bOption = true;
    m_ceImpliedVolatility.Clear();
    m_ceDelta.Clear();
    m_ceGamma.Clear();
    m_ceTheta.Clear();
    m_ceVega.Clear();
  }

  return m_legNote.Values();
}

Leg::pPosition_t Leg::GetPosition() const { return m_pPosition; }

void Leg::Tick( ptime dt, double dblUnderlyingPrice ) {
  if ( m_pPosition ) {
    double dblPL = m_pPosition->GetRealizedPL() + m_pPosition->GetUnRealizedPL() - m_pPosition->GetCommissionPaid();
    m_ceProfitLoss.Append( dt, dblPL );
    if ( m_bOption ) {
      ou::tf::Watch::pWatch_t pWatch = m_pPosition->GetWatch();
      ou::tf::option::Option::pOption_t pOption = std::dynamic_pointer_cast<ou::tf::option::Option>( pWatch );
      m_ceImpliedVolatility.Append( dt, pOption->ImpliedVolatility() );
      m_ceDelta.Append( dt, pOption->Delta() );
      m_ceGamma.Append( dt, pOption->Gamma() );
      m_ceTheta.Append( dt, pOption->Theta() );
      m_ceVega.Append( dt, pOption->Vega() );
    }
  }
}

bool Leg::IsActive() const {
  bool bIsActive( false );
  if ( m_pPosition ) {
    bIsActive = m_pPosition->IsActive();
  }
  return bIsActive;
}

void Leg::SaveSeries( const std::string& sPrefix ) {
  if ( m_pPosition ) {
    if ( m_bOption ) {
      ou::tf::Watch::pWatch_t pWatch = m_pPosition->GetWatch();
      ou::tf::option::Option::pOption_t pOption = std::dynamic_pointer_cast<ou::tf::option::Option>( pWatch );
      pOption->SaveSeries( sPrefix );
    }
    else {
      m_pPosition->GetWatch()->SaveSeries( sPrefix );
    }
  }
}

namespace {
  constexpr size_t ixPL = 2;
  constexpr size_t ixIV = 11;
  constexpr size_t ixDelta = 12;
  constexpr size_t ixGamma = 13;
  constexpr size_t ixTheta = 14;
  constexpr size_t ixVega = 15;
}

void Leg::SetChartData( pChartDataView_t pChartDataView, ou::Colour::EColour colour ) {

  DelChartData();

  if ( m_pPosition ) {
    const std::string& sInstrumentNmae( m_pPosition->GetInstrument()->GetInstrumentName() );
    m_ceProfitLoss.SetName( "P/L: " + sInstrumentNmae );
    m_ceProfitLoss.SetColour( colour );
    pChartDataView->Add( ixPL, &m_ceProfitLoss );

    if ( m_bOption ) {
      m_ceImpliedVolatility.SetName( "IV: " + sInstrumentNmae );
      m_ceImpliedVolatility.SetColour( colour );
      pChartDataView->Add( ixIV, &m_ceImpliedVolatility );

      m_ceDelta.SetName( "Delta: " + sInstrumentNmae );
      m_ceDelta.SetColour( colour );
      pChartDataView->Add( ixDelta, &m_ceDelta );

      m_ceGamma.SetName( "Gamma: " + sInstrumentNmae );
      m_ceGamma.SetColour( colour );
      pChartDataView->Add( ixGamma, &m_ceGamma );

      m_ceTheta.SetName( "Theta: " + sInstrumentNmae );
      m_ceTheta.SetColour( colour );
      pChartDataView->Add( ixTheta, &m_ceTheta );

      m_ceVega.SetName( "Vega: " + sInstrumentNmae );
      m_ceVega.SetColour( colour );
      pChartDataView->Add( ixVega, &m_ceVega );
    }
    m_pChartDataView = pChartDataView;
  }
}

void Leg::DelChartData() {
  //if ( m_pPosition ) {
    if ( m_pChartDataView ) {

      m_pChartDataView->Remove( ixPL, &m_ceProfitLoss );
      m_ceProfitLoss.Clear();

      if ( m_bOption ) {

        m_pChartDataView->Remove( ixIV, &m_ceImpliedVolatility );
        m_ceImpliedVolatility.Clear();

        m_pChartDataView->Remove( ixDelta, &m_ceDelta );
        m_ceDelta.Clear();

        m_pChartDataView->Remove( ixGamma, &m_ceGamma );
        m_ceGamma.Clear();

        m_pChartDataView->Remove( ixTheta, &m_ceTheta );
        m_ceTheta.Clear();

        m_pChartDataView->Remove( ixVega, &m_ceVega );
        m_ceVega.Clear();
      }

      m_pChartDataView.reset();
    }
  //}
}

double Leg::GetNet( double price ) const {
  double dblValue_UnRealized {};
  if ( m_pPosition ) {

    ou::tf::OrderSide::EOrderSide eOrderSide( m_pPosition->GetRow().eOrderSideActive );

    std::cout
      << "  leg: "
      << m_pPosition->GetInstrument()->GetInstrumentName()
      ;

    if ( ou::tf::OrderSide::Unknown == eOrderSide ) {
      double dblValue_Realized = m_pPosition->GetRealizedPL();
      std::cout << ",realized=" << dblValue_Realized;
    }
    else {
      dblValue_UnRealized = m_pPosition->GetUnRealizedPL();
      std::string sign;
      switch ( eOrderSide ) {
        case ou::tf::OrderSide::Buy:
          sign = "+";
          break;
        case ou::tf::OrderSide::Sell:
          sign = "-";
          break;
        default:
          assert( false );
      }

      std::cout
        << "=>"
        << sign
        << m_pPosition->GetActiveSize()
        << "@"
        << dblValue_UnRealized;

      if ( m_bOption ) {
        pOption_t pOption = std::dynamic_pointer_cast<ou::tf::option::Option>( m_pPosition->GetWatch() );
        switch ( pOption->GetInstrument()->GetOptionSide() ) {
          case ou::tf::OptionSide::Call:
            if ( price >= m_pPosition->GetInstrument()->GetStrike() ) {
              std::cout << "(ITM)";
            }
            else {
              std::cout << "(otm)";
            }
            break;
          case ou::tf::OptionSide::Put:
            if ( price <= m_pPosition->GetInstrument()->GetStrike() ) {
              std::cout << "(ITM)";
            }
            else {
              std::cout << "(otm)";
            }
            break;
          default:
            assert( false );
        }
        std::cout << ",";
        pOption->EmitValues( price, false );
        std::cout << ",";
      }
    }
  }
  return dblValue_UnRealized;
}

void Leg::NetGreeks( double& delta, double& gamma ) const {
  if ( m_pPosition ) {
    double quantity {};
    switch ( m_pPosition->GetRow().eOrderSideActive ) {
      case ou::tf::OrderSide::Buy:
        quantity = 1.0 * m_pPosition->GetActiveSize();
        break;
      case ou::tf::OrderSide::Sell:
        quantity = -1.0 * m_pPosition->GetActiveSize();
        break;
      case ou::tf::OrderSide::Unknown:
        // not active
        break;
      default:
        assert( false );
    }

    if ( 0.0 != quantity ) {
      if ( m_bOption ) {
        pOption_t pOption = std::dynamic_pointer_cast<ou::tf::option::Option>( m_pPosition->GetWatch() );
        pOption->NetGreeks( quantity, delta, gamma );
      }
    }
  }
}

double Leg::ConstructedValue() const {
  double value {};
  if ( m_pPosition ) {
    const ou::tf::Position::TableRowDef& row( m_pPosition->GetRow() );
    value = row.nPositionActive * row.dblConstructedValue / m_pPosition->GetInstrument()->GetMultiplier();
    switch ( row.eOrderSideActive ) {
      case ou::tf::OrderSide::Buy:
        break;
      case ou::tf::OrderSide::Sell:
        value = -value; // turns the value positive, TODO use the negative at some point
        break;
      case ou::tf::OrderSide::Unknown:
        break; // position is closed
      default:
        assert( false );
    }
  }
  return value;
}

} // namespace ou
} // namespace tf
