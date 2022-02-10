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

#include "ChartEntryPrice.h"

// need to add in std::string for commenting the shape
// vectors need to be time ordered

namespace ou { // One Unified

class ChartEntryShape :
  public ChartEntryPrice {
public:
  enum enumShape { EDefault, ELong, EShort, EFillLong, EFillShort, ELongStop, EShortStop };
  ChartEntryShape();
  ChartEntryShape( enumShape eShape, ou::Colour::enumColour colour );
  void SetShape( enumShape shape ) { m_eShape = shape; };
  enumShape GetShape() const { return m_eShape; };
  virtual ~ChartEntryShape(void);
  void AddLabel( const boost::posix_time::ptime &dt, double price, const std::string &sLabel );
  virtual bool AddEntryToChart( XYChart* pXY, structChartAttributes* pAttributes );
  virtual void Clear();
  void ClearQueue();
protected:

  struct Entry {
    const ou::tf::Price price;
    const char* pLabel;
    Entry(): pLabel( 0 ) {}
    Entry( const ou::tf::Price& price_, const char* pLabel_ ): price( price_ ), pLabel( pLabel_ ) {}
  };

  using vpChar_t = std::vector<const char*>;
  vpChar_t m_vpChar;

  static int m_rShapes[];  // constants
  enumShape m_eShape;
  StringArray GetLabels() const {
    //std::vector<const char *>::const_iterator iter = m_vpChar.begin();
    //return StringArray( &(*iter), static_cast<int>( m_vpChar.size() ) );
    return StringArray( &m_vpChar[ m_ixStart ],  m_nElements );
  }
private:
  using queueLabel_t = ou::tf::Queue<Entry>;
  queueLabel_t m_queueLabel;
  //boost::lockfree::spsc_queue<char*, boost::lockfree::capacity<lockfreesize> > m_lfShape;
  void Pop( const Entry& );
};

} // namespace ou
