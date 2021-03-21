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

#include "Leg.h"

namespace ou {
namespace tf {

Leg::Leg()
: m_bOption( false )
{
  Init();
}

Leg::Leg( pPosition_t pPosition ) // implies candidate will not be used
: m_bOption( false )
{
  Init();
  SetPosition( pPosition );
}

Leg::Leg( Leg&& rhs )
: m_pPosition( std::move( rhs.m_pPosition ) ),
  m_monitor( std::move( rhs.m_monitor ) ),
  m_legNote( std::move( rhs.m_legNote ) ),
  m_bOption( rhs.m_bOption ),
  m_pChartDataView( std::move( rhs.m_pChartDataView ) )
{
  Init();
}

Leg& Leg::operator=( const Leg&& rhs ) {
  if ( this != &rhs ) {
    m_pPosition = std::move( rhs.m_pPosition );
    m_monitor = std::move( rhs.m_monitor );
    m_legNote = std::move( rhs.m_legNote );
    m_bOption = rhs.m_bOption;
    m_pChartDataView = std::move( rhs.m_pChartDataView );
    Init();
  }
  return *this;
}

Leg::~Leg() {
  if ( m_pPosition ) {
    std::cout << "Leg destruction: " << m_pPosition->GetInstrument()->GetInstrumentName() << std::endl;
  }
  else {
    std::cout << "Leg destruction: unknown" << std::endl;
  }
  DelChartData();
  assert( !m_monitor.IsOrderActive() );
}

const ou::tf::option::LegNote::values_t& Leg::SetPosition( pPosition_t pPosition ) {

  if ( m_pPosition ) {
    std::cout
      << "Leg::SetPosition over-write position "
      << m_pPosition->GetInstrument()->GetInstrumentName()
      << " with "
      << pPosition->GetInstrument()->GetInstrumentName()
      << std::endl;

    if ( m_monitor.IsOrderActive() ) {
      std::cout
        << "Leg::SetPosition cancelling order for position "
        << m_pPosition->GetInstrument()->GetInstrumentName()
        << std::endl;
      m_monitor.CancelOrder();
      while ( m_monitor.IsOrderActive() );  // hopeufully this doesn't lock
    }
  }

  std::cout << "Leg::SetPosition: " << m_pPosition->GetInstrument()->GetInstrumentName() << std::endl;

  m_pPosition = pPosition;
  m_legNote.Decode( m_pPosition->Notes() );

  m_monitor.SetPosition( m_pPosition );

  ou::tf::Watch::pWatch_t pWatch = m_pPosition->GetWatch();
  // NOTE: this may generate error with non-option!
  ou::tf::option::Option::pOption_t pOption = boost::dynamic_pointer_cast<ou::tf::option::Option>( pWatch );

  m_bOption = false;
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

void Leg::Tick( ptime dt ) {
  m_monitor.Tick( dt );
  if ( m_pPosition ) {
    double dblPL = m_pPosition->GetRealizedPL() + m_pPosition->GetUnRealizedPL() - m_pPosition->GetCommissionPaid();
    m_ceProfitLoss.Append( dt, dblPL );
    if ( m_bOption ) {
      ou::tf::Watch::pWatch_t pWatch = m_pPosition->GetWatch();
      ou::tf::option::Option::pOption_t pOption = boost::dynamic_pointer_cast<ou::tf::option::Option>( pWatch );
      m_ceImpliedVolatility.Append( dt, pOption->ImpliedVolatility() );
      m_ceDelta.Append( dt, pOption->Delta() );
      m_ceGamma.Append( dt, pOption->Gamma() );
      m_ceTheta.Append( dt, pOption->Theta() );
      m_ceVega.Append( dt, pOption->Vega() );
    }
  }
}

void Leg::PlaceOrder( ou::tf::OrderSide::enumOrderSide side, boost::uint32_t nOrderQuantity ) {
  if ( m_pPosition ) {
    m_monitor.PlaceOrder( nOrderQuantity, side );
  }
}

void Leg::CancelOrder() {
  if ( m_pPosition ) {
    m_monitor.CancelOrder();
  }
}

void Leg::ClosePosition() {
  if ( m_pPosition ) {
    m_monitor.ClosePosition();
  }
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
      ou::tf::option::Option::pOption_t pOption = boost::dynamic_pointer_cast<ou::tf::option::Option>( pWatch );
      pOption->SaveSeries( sPrefix );
    }
    else {
      m_pPosition->GetWatch()->SaveSeries( sPrefix );
    }
  }
}

void Leg::SetColour( ou::Colour::enumColour colour ) {
  m_ceProfitLoss.SetColour( colour );
  m_ceImpliedVolatility.SetColour( colour );
  m_ceDelta.SetColour( colour );
  m_ceGamma.SetColour( colour );
  m_ceTheta.SetColour( colour );
  m_ceVega.SetColour( colour );
}

void Leg::SetChartData( pChartDataView_t pChartDataView ) {
  DelChartData();
  if ( m_pPosition ) {
    m_ceProfitLoss.SetName( "P/L: " + m_pPosition->GetInstrument()->GetInstrumentName() );
    pChartDataView->Add( 2, &m_ceProfitLoss );

    if ( m_bOption ) {
      m_ceImpliedVolatility.SetName( "IV: " + m_pPosition->GetInstrument()->GetInstrumentName() );
      pChartDataView->Add( 11, &m_ceImpliedVolatility );
      m_ceDelta.SetName( "Delta: " + m_pPosition->GetInstrument()->GetInstrumentName() );
      pChartDataView->Add( 12, &m_ceDelta );
      m_ceGamma.SetName( "Gamma: " + m_pPosition->GetInstrument()->GetInstrumentName() );
      pChartDataView->Add( 13, &m_ceGamma );
      m_ceTheta.SetName( "Theta: " + m_pPosition->GetInstrument()->GetInstrumentName() );
      pChartDataView->Add( 14, &m_ceTheta );
      m_ceVega.SetName( "Vega: " + m_pPosition->GetInstrument()->GetInstrumentName() );
      pChartDataView->Add( 15, &m_ceVega );
    }
    m_pChartDataView = pChartDataView;
  }
}

void Leg::DelChartData() {
  //if ( m_pPosition ) {
    if ( m_pChartDataView ) {
      m_pChartDataView->Remove( 2, &m_ceProfitLoss );
      if ( m_bOption ) {
        m_pChartDataView->Remove( 11, &m_ceImpliedVolatility );
        m_pChartDataView->Remove( 12, &m_ceDelta );
        m_pChartDataView->Remove( 13, &m_ceGamma );
        m_pChartDataView->Remove( 14, &m_ceTheta );
        m_pChartDataView->Remove( 15, &m_ceVega );
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
    pOption_t pOption = boost::dynamic_pointer_cast<ou::tf::option::Option>( m_pPosition->GetWatch() );
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
    pOption_t pOption = boost::dynamic_pointer_cast<ou::tf::option::Option>( m_pPosition->GetWatch() );
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
  double dblValue {};
  if ( m_pPosition ) {
    dblValue = m_pPosition->GetUnRealizedPL();
    std::cout
      << "  leg: "
      << m_pPosition->GetInstrument()->GetInstrumentName()
      << "=>"
      << m_pPosition->GetActiveSize()
      << "@"
      << dblValue;
    if ( m_bOption ) {
      pOption_t pOption = boost::dynamic_pointer_cast<ou::tf::option::Option>( m_pPosition->GetWatch() );
      switch ( pOption->GetInstrument()->GetOptionSide() ) {
        case ou::tf::OptionSide::Call:
          if ( price > m_pPosition->GetInstrument()->GetStrike() ) {
            std::cout << "(ITM)";
          }
          if ( price < m_pPosition->GetInstrument()->GetStrike() ) {
            std::cout << "(otm)";
          }
          break;
        case ou::tf::OptionSide::Put:
          if ( price < m_pPosition->GetInstrument()->GetStrike() ) {
            std::cout << "(ITM)";
          }
          if ( price > m_pPosition->GetInstrument()->GetStrike() ) {
            std::cout << "(otm)";
          }
          break;
      }
    }
//    if ( 0.0 == dblValue ) {
      const ou::tf::Quote& quote( m_pPosition->GetWatch()->LastQuote() );
      std::cout
        << ",b" << quote.Bid()
        << ",a" << quote.Ask()
        ;
//    }
//    std::cout << std::endl;
  }
  return dblValue;
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
    }
  }
  return value;
}

void Leg::Init() {
}

} // namespace ou
} // namespace tf
