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

class CChartEntryShape :
  public CChartEntryBaseWithTime {
public:
  enum enumShape { EDefault, EBuy, ESell, EFillLong, EFillShort, EBuyStop, ESellStop };
  CChartEntryShape( void );
  CChartEntryShape( enumShape eShape, Colour::enumColour colour );
  void SetShape( enumShape shape ) { m_eShape = shape; };
  enumShape GetShape( void ) { return m_eShape; };
  virtual ~CChartEntryShape(void);
  void AddLabel( const ptime &dt, double price, const std::string &sLabel );
  virtual void AddDataToChart( XYChart *pXY, structChartAttributes *pAttributes );
protected:
  static int m_rShapes[];
  enumShape m_eShape;
  //std::vector<std::string> m_vLabel;
  std::vector<const char *> m_vpChar;
  StringArray GetLabels( void ) {
    std::vector<const char *>::iterator iter = m_vpChar.begin();
    return StringArray( &(*iter), static_cast<int>( m_vpChar.size() ) );
  }
private:
};
