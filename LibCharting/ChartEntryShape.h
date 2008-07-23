#pragma once

#include "ChartEntryBase.h"
#include "Colour.h"

#include <vector>
#include <string>

// need to add in std::string for commenting the shape

class CChartEntryShape :
  public CChartEntryBaseWithTime {
public:
  enum EShape { EBuy, ESell, EBuyStop, ESellStop, EFillLong, EFillShort };
  CChartEntryShape( EShape eShape, Colour::enumColour colour );
  virtual ~CChartEntryShape(void);
  void AddLabel( const ptime &dt, double price, const std::string &sLabel );
  virtual void AddDataToChart( XYChart *pXY );
protected:
  static int m_rShapes[];
  EShape m_eShape;
  Colour::enumColour m_colour;
  std::vector<std::string> m_vLabel;
  std::vector<const char *> m_vpChar;
  StringArray GetLabels( void ) {
    std::vector<const char *>::iterator iter = m_vpChar.begin();
    return StringArray( &(*iter), static_cast<int>( m_vpChar.size() ) );
  }
private:
};
