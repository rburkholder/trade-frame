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
: m_pPosition( std::move( rhs.m_pPosition ) ),
  m_monitor( std::move( rhs.m_monitor ) ),
  m_legNote( std::move( rhs.m_legNote ) ),
  m_bOption( rhs.m_bOption ),
  m_pChartDataView( std::move( rhs.m_pChartDataView ) )
{
}

Leg& Leg::operator=( Leg&& rhs ) {
  if ( this != &rhs ) {
    DelChartData();
    m_pPosition = std::move( rhs.m_pPosition );
    m_monitor = std::move( rhs.m_monitor );
    m_legNote = std::move( rhs.m_legNote );
    m_bOption = rhs.m_bOption;
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
  assert( !m_monitor.IsOrderActive() );
}

const ou::tf::option::LegNote::values_t& Leg::SetPosition( pPosition_t pPosition ) {

  if ( m_pPosition ) {
    BOOST_LOG_TRIVIAL(info)
      << "Leg::SetPosition over-write position "
      << m_pPosition->GetInstrument()->GetInstrumentName()
      << " with "
      << pPosition->GetInstrument()->GetInstrumentName()
      ;

    if ( m_monitor.IsOrderActive() ) {
      BOOST_LOG_TRIVIAL(info)
        << "Leg::SetPosition cancelling order for position "
        << m_pPosition->GetInstrument()->GetInstrumentName()
        ;
      m_monitor.CancelOrder();
      while ( m_monitor.IsOrderActive() );  // hopeufully this doesn't lock
    }
    DelChartData();
    m_pPosition.reset();
    m_bOption = false;
  }

  BOOST_LOG_TRIVIAL(info) << "Leg::SetPosition: " << pPosition->GetInstrument()->GetInstrumentName();

  m_pPosition = pPosition;
  m_legNote.Decode( m_pPosition->Notes() );

  m_monitor.SetPosition( m_pPosition );

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
  m_monitor.Tick( dt );
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

void Leg::PlaceOrder( ou::tf::OrderSide::EOrderSide side, boost::uint32_t nOrderQuantity ) {
  if ( m_pPosition ) {
    m_monitor.PlaceOrder( nOrderQuantity, side );
  }
}

void Leg::CancelOrder() {
  if ( m_pPosition ) {
    m_monitor.CancelOrder();
  }
}

Leg::pPosition_t Leg::ClosePosition() {
  if ( m_pPosition ) {
    // m_legNote.Decode( m_pPosition->Notes() ); // TODO: validate legn notes are undisturbed
    //option::LegNote::values_t values = m_legNote.Values();
    //values.m_state = option::LegNote::State::Closed;
    //m_legNote.Assign( values );
    assert ( option::LegNote::State::Closed != m_legNote.GetState() );
    m_legNote.SetState( option::LegNote::State::Closed );
    m_pPosition->SetNotes( m_legNote.Encode() );
    m_monitor.ClosePosition();
  }
  return m_pPosition;
}

bool Leg::IsActive() const {
  bool bIsActive( false );
  if ( m_pPosition ) {
    bIsActive = m_pPosition->IsActive();
  }
  return bIsActive;
}

bool Leg::IsOrderActive() const { return m_monitor.IsOrderActive(); }

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

bool Leg::CloseItm( const double price ) {
  bool bClosed( false );
  if ( m_pPosition ) {
    if ( m_bOption ) {
      if ( m_pPosition->IsActive() ) {
        using pInstrument_t = Position::pInstrument_t;
        pInstrument_t pInstrument = m_pPosition->GetInstrument();
        switch ( pInstrument->GetOptionSide() ) {
          case ou::tf::OptionSide::Call:
            if ( price > pInstrument->GetStrike() ) {
              ClosePosition();
              bClosed = true;
            }
            break;
          case ou::tf::OptionSide::Put:
            if ( price < pInstrument->GetStrike() ) {
              ClosePosition();
              bClosed = true;
            }
            break;
        }
      }
    }
  }
  return bClosed;
}

bool Leg::CloseItmForProfit( const double price ) {
  bool bClosed( false );
  if ( m_pPosition ) {
    if ( m_bOption ) {
      if ( m_pPosition->IsActive() ) {
        using pInstrument_t = Position::pInstrument_t;
        pInstrument_t pInstrument = m_pPosition->GetInstrument();
        switch ( pInstrument->GetOptionSide() ) {
          case ou::tf::OptionSide::Call:
            if ( price > pInstrument->GetStrike() ) {
              if ( ( 100 * m_pPosition->GetActiveSize() * 0.05 ) < m_pPosition->GetUnRealizedPL() ) {
                ClosePosition();
                bClosed = true;
              }
            }
            break;
          case ou::tf::OptionSide::Put:
            if ( price < pInstrument->GetStrike() ) {
              if ( ( 100 * m_pPosition->GetActiveSize() * 0.05 ) < m_pPosition->GetUnRealizedPL() ) {
                ClosePosition();
                bClosed = true;
              }
            }
            break;
        }
      }
    }
  }
  return bClosed;
}

void Leg::CloseExpiryItm( const boost::gregorian::date date, const double price ) {
  using pInstrument_t = Position::pInstrument_t;
  if ( m_pPosition ) {
    pOption_t pOption = std::dynamic_pointer_cast<ou::tf::option::Option>( m_pPosition->GetWatch() );
    pInstrument_t pInstrument = pOption->GetInstrument();
    if ( date == pInstrument->GetExpiry() ) {
      const double strike = pInstrument->GetStrike();
      switch ( pInstrument->GetOptionSide() ) {
        case OptionSide::Call:
          if ( price >= strike ) {
            ClosePosition();
          }
          break;
        case OptionSide::Put:
          if ( price <= strike ) {
            ClosePosition();
          }
          break;
      }
    }
  }
}

void Leg::CloseExpiryOtm( const boost::gregorian::date date, double price ) {
  using pInstrument_t = Position::pInstrument_t;
  if ( m_pPosition ) {
    pOption_t pOption = std::dynamic_pointer_cast<ou::tf::option::Option>( m_pPosition->GetWatch() );
    pInstrument_t pInstrument = pOption->GetInstrument();
    if ( date == pInstrument->GetExpiry() ) {
      const double strike = pInstrument->GetStrike();
      switch ( pInstrument->GetOptionSide() ) {
        case OptionSide::Call:
          if ( price < strike ) {
            ClosePosition();
          }
          break;
        case OptionSide::Put:
          if ( price > strike ) {
            ClosePosition();
          }
          break;
      }
    }
  }
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
      default:
        assert( false );
    }

    if ( m_bOption ) {
      pOption_t pOption = std::dynamic_pointer_cast<ou::tf::option::Option>( m_pPosition->GetWatch() );
      pOption->NetGreeks( quantity, delta, gamma );
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
      default:
        assert( false );
    }
  }
  return value;
}

} // namespace ou
} // namespace tf
