/************************************************************************
 * Copyright(c) 2023, One Unified. All rights reserved.                 *
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
 * File:    HiPass.cpp
 * Author:  raymond@burkholder.net
 * Project: rdaf/l2
 * Created: February 6, 2023  18:54:43
 */

 #include "HiPass.hpp"

HiPass::HiPass()
: m_alpha( 0.1 )
, m_dblPrice0 {}, m_dblPrice1 {}, m_dblPrice2 {}
, m_dblHPF0 {}, m_dblHPF1 {}, m_dblHPF2 {}
//, m_dblLPF0 {}, m_dblLPF1 {}, m_dblLPF2 {}
, m_dblHPF_Slope0 {}, m_dblHPF_Slope1 {}
{}


void HiPass::Init( int nPeriods, ou::Colour::EColour colour, const std::string& sName ) {

  m_alpha = 1.0 / (double) nPeriods;
  m_one_minus_alpha = 1.0 - m_alpha;
  m_alpha_by_two = m_alpha / 2.0;
  m_one_minus_alpha_by_two = 1.0 - m_alpha_by_two;
  m_alpha_squared = m_alpha * m_alpha;

  m_ceEhlersHiPassFilter.SetColour( colour );
  m_ceEhlersHiPassFilterSlope.SetColour( colour );
  //m_ceEhlersLoPassFilter.SetColour( colour );

  m_ceEhlersHiPassFilter.SetName( sName );
  m_ceEhlersHiPassFilterSlope.SetName( sName + "Slope" );
  //m_ceEhlersLoPassFilter.SetName( "LoPass" );

}

void HiPass::Update( boost::posix_time::ptime dt, double price ) {
  // ehlers page 15, eqn 2.7, high pass filter
  // ehlers page 16, eqn 2.9, low pass filter

  if ( 0.0 == m_dblPrice0 ) {
    m_dblPrice0 = m_dblPrice1 = m_dblPrice2 = price;
    //m_dblHPF0 = m_dblHPF1 = m_dblHPF2 = 0.0;
    //m_dblLPF0 = m_dblLPF1 = m_dblLPF2 = ma0;
  }
  else {
    m_dblPrice2 = m_dblPrice1; m_dblPrice1 = m_dblPrice0; // archive older values
    m_dblPrice0 = price;
    const double weighted = m_dblPrice0 - ( m_dblPrice1 + m_dblPrice1 ) + m_dblPrice2;

    m_dblHPF2 = m_dblHPF1; m_dblHPF1 = m_dblHPF0; // archive older values

    m_dblHPF0 = m_one_minus_alpha_by_two * m_one_minus_alpha_by_two * weighted
              + 2.0 * m_one_minus_alpha * m_dblHPF1
              - m_one_minus_alpha * m_one_minus_alpha * m_dblHPF2
              ;
    m_ceEhlersHiPassFilter.Append( dt, m_dblHPF0 );

    m_dblHPF_Slope1 = m_dblHPF_Slope0; // keep previous value
    m_dblHPF_Slope0 = m_dblHPF0 - m_dblHPF1; // diff is slope
    m_ceEhlersHiPassFilterSlope.Append( dt, m_dblHPF_Slope0 );

    //if ( 10 > m_ceEhlersHiPassFilter.Size() ) {
    //  BOOST_LOG_TRIVIAL(info) << "hpf=" << m_dblPrice0 << "," << m_dblHPF0 << "," << m_dblHPF1 << "," << m_dblHPF2 << std::endl;
    //}

    // only marginally better than a simple ema
    //m_dblLPF2 = m_dblLPF1; m_dblLPF1 = m_dblLPF0;
    //m_dblLPF0 = ( m_alpha - m_alpha_by_two * m_alpha_by_two ) * m_dblPrice0
    //          + ( m_alpha_squared / 2 ) * m_dblPrice1
    //          - ( m_alpha - 3.0 * m_alpha_squared / 4.0 ) * m_dblPrice2
    //          + 2.0 * m_one_minus_alpha * m_dblLPF1
    //          - m_one_minus_alpha * m_one_minus_alpha * m_dblLPF2
    //          ;
    //m_ceEhlersLoPassFilter.Append( dt, m_dblLPF0 );
  }

}
