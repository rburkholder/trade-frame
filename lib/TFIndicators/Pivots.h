/************************************************************************
 * Copyright(c) 2010, One Unified. All rights reserved.                 *
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

#pragma once

#include <string>

#include <OUCommon/Colour.h>

#include <TFTimeSeries/TimeSeries.h>

namespace ou { // One Unified
namespace tf { // TradeFrame

class PivotSet {
public:

  enum EPivots { R3, R23, R2, R12, R1, PVR1, PV, PVS1, S1, S12, S2, S23, S3, PivotCount };

  struct structPivotInfo {
    std::string sName;
    ou::Colour::EColour colour;
    structPivotInfo() : colour( ou::Colour::Black ) {};
    structPivotInfo( const std::string &sName_, ou::Colour::EColour colour_ )
      : sName( sName_ ), colour( colour_ ) {};
  };

  PivotSet();
  PivotSet( const std::string &sName, double S3, double S2, double S1, double PV, double R1, double R2, double R3 );
  PivotSet( const std::string &sName, double Hi, double Lo, double Close );
  PivotSet( const std::string &sName, const Bar& bar );
  PivotSet( const std::string &sName, Bars* bars );
  // add in  a constructor with bar iterators, can then do pivot for weekly bar set or monthly bar set, etc

  virtual ~PivotSet();

  void CalcPivots( const std::string &sName, double Hi, double Lo, double Close );
  void CalcPivots( double Hi, double Lo, double Close );
  void CalcPivots( const Bar& bar );
  void CalcHalfPivots();

  using pivot_t = std::pair<double, structPivotInfo>;

  const std::string &Name( void ) const { return m_sName; };

  pivot_t operator[]( unsigned short ix ) { // this probably copies twice or thrice in succession
    assert( ix >= R3 );
    assert( ix <= S3 );
    structPivotInfo pivot( m_sName + m_sPivotNames[ ix ], m_rPivotColours[ ix ] );
    return pivot_t( m_rPivots[ ix ], pivot );
  }
  double GetPivotValue( EPivots ix ) const {
    assert( ix >= R3 );
    assert( ix <= S3 );
    return m_rPivots[ ix ];
  };
  std::string GetPivotName( EPivots ix ) const {
    assert( ix >= R3 );
    assert( ix <= S3 );
    return m_sName + m_sPivotNames[ ix ];
  };
  static ou::Colour::EColour GetPivotColour( EPivots ix ) {
    assert( ix >= R3 );
    assert( ix <= S3 );
    return m_rPivotColours[ ix ];
  };

protected:

private:

  std::string m_sName;

  double m_rPivots[ PivotCount ];
  static const std::string m_sPivotNames[ PivotCount ];
  static const ou::Colour::EColour m_rPivotColours[ PivotCount ];

};

} // namespace tf
} // namespace ou
