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
 * Project: TFBitsNPieces
 * Created on May 25, 2019, 4:46 PM
 */

#include "Leg.h"

namespace ou {
namespace tf {

Leg::Leg() {
  Init();
}

Leg::Leg( pPosition_t pPosition ) // implies candidate will not be used
{
  Init();
  SetPosition( pPosition );
}

Leg::Leg( const Leg&& rhs )
: m_pPosition( std::move( rhs.m_pPosition ) ),
  m_monitor( std::move( rhs.m_monitor ) )
{
  Init();
}

void Leg::SetPosition( pPosition_t pPosition ) {
  m_pPosition = pPosition;
  m_monitor.SetPosition( pPosition );
  m_ceProfitLoss.SetName( m_pPosition->GetInstrument()->GetInstrumentName() + " P/L" );
}

Leg::pPosition_t Leg::GetPosition() { return m_pPosition; }

void Leg::Tick( ptime dt ) {
  m_monitor.Tick();
  if ( m_pPosition ) {
    double dblPL = m_pPosition->GetRealizedPL() + m_pPosition->GetUnRealizedPL() - m_pPosition->GetCommissionPaid();
    m_ceProfitLoss.Append( dt, dblPL );
  }
}

void Leg::OrderLong( boost::uint32_t nOrderQuantity ) {
  if ( m_pPosition ) {
    m_monitor.PlaceOrder( nOrderQuantity, ou::tf::OrderSide::Buy );
  }

}
void Leg::OrderShort( boost::uint32_t nOrderQuantity ) {
  if ( m_pPosition ) {
    m_monitor.PlaceOrder( nOrderQuantity, ou::tf::OrderSide::Sell );
  }

}
void Leg::CancelOrder() {
  if ( m_pPosition ) {
    m_monitor.CancelOrder();
  }
}
void Leg::ClosePosition() {
  if ( m_pPosition ) {
    const ou::tf::Position::TableRowDef& row( m_pPosition->GetRow() );
    if ( m_monitor.IsOrderActive() ) {
      std::cout << row.sName << ": error, monitor has active order, no close possible" << std::endl;
    }
    else {
      if ( 0 != row.nPositionPending ) {
        std::cout << row.sName << ": warning, has pending size of " << row.nPositionPending << " during close" << std::endl;
      }
      if ( 0 != row.nPositionActive ) {
        std::cout << row.sName << ": closing position" << std::endl;
        switch ( row.eOrderSideActive ) {
          case ou::tf::OrderSide::Buy:
            m_monitor.PlaceOrder( row.nPositionActive, ou::tf::OrderSide::Sell );
            break;
          case ou::tf::OrderSide::Sell:
            m_monitor.PlaceOrder( row.nPositionActive, ou::tf::OrderSide::Buy );
            break;
        }
      }
    }
  }
}
bool Leg::IsOrderActive() const { return m_monitor.IsOrderActive(); }

void Leg::SaveSeries( const std::string& sPrefix ) {
  if ( m_pPosition ) {
    m_pPosition->GetWatch()->SaveSeries( sPrefix );
  }
}

void Leg::SetColour( ou::Colour::enumColour colour ) { m_ceProfitLoss.SetColour( colour ); }

void Leg::AddChartData( pChartDataView_t pChartData ) {
  pChartData->Add( 2, &m_ceProfitLoss );
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

void Leg::Init() {
}

} // namespace ou
} // namespace tf
