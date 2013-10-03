/************************************************************************
 * Copyright(c) 2010, One Unified. All rights reserved.                 *
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

#include <vector>
#include <string>

#include "ChartEntryBase.h"

// need to add in std::string for commenting the shape

namespace ou { // One Unified

class ChartEntryShape :
  public ChartEntryBaseWithTime {
public:
  enum enumShape { EDefault, EBuy, ESell, EFillLong, EFillShort, EBuyStop, ESellStop };
  ChartEntryShape( void );
  ChartEntryShape( enumShape eShape, ou::Colour::enumColour colour );
  void SetShape( enumShape shape ) { m_eShape = shape; };
  enumShape GetShape( void ) const { return m_eShape; };
  virtual ~ChartEntryShape(void);
  void AddLabel( const boost::posix_time::ptime &dt, double price, const std::string &sLabel );
  virtual void AddEntryToChart( XYChart *pXY, structChartAttributes *pAttributes ) const;
protected:
  static int m_rShapes[];
  enumShape m_eShape;
  //std::vector<std::string> m_vLabel;
  std::vector<const char *> m_vpChar;
  StringArray GetLabels( void ) const {
    std::vector<const char *>::const_iterator iter = m_vpChar.begin();
    return StringArray( &(*iter), static_cast<int>( m_vpChar.size() ) );
  }
private:
};

} // namespace ou
