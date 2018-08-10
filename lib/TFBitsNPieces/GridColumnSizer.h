/************************************************************************
 * Copyright(c) 2018, One Unified. All rights reserved.                 *
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
 * File:   GridColumnSizer.h
 * Author: raymond@burkholder.net
 *
 * Created on August 9, 2018, 2:07 PM
 */

#ifndef GRIDCOLUMNSIZER_H
#define GRIDCOLUMNSIZER_H

#include <vector>

#include <boost/serialization/version.hpp>
#include <boost/serialization/split_member.hpp>

#include <wx/grid.h>

namespace ou { // One Unified
namespace tf { // TradeFrame

class GridColumnSizer {
  friend class boost::serialization::access;
public:
  
  GridColumnSizer( );
  virtual ~GridColumnSizer( );
  
  void SaveColumnSizes( const wxGrid& );
  void SetColumnSizes( wxGrid& );
  
protected:
private:
  
  typedef std::vector<int> vColumnSize_t;
  
  vColumnSize_t m_vColumnSize;
  
  template<typename Archive>
  void save( Archive& ar, const unsigned int version ) const {
    size_t cnt = m_vColumnSize.size();
    ar & cnt;
    for ( int ix = 0; ix < cnt; ix++ ) {
      ar & m_vColumnSize[ ix ];
    }
  }

  template<typename Archive>
  void load( Archive& ar, const unsigned int version ) {
    size_t cnt;
    ar & cnt;
    m_vColumnSize.clear();
    int width;
    for ( int ix = 0; ix < cnt; ix++ ) {
      ar & width;
      m_vColumnSize.push_back( width );
    }
  }
  
  BOOST_SERIALIZATION_SPLIT_MEMBER()

};

} // namespace tf
} // namespace ou

BOOST_CLASS_VERSION(ou::tf::GridColumnSizer, 1)

#endif /* GRIDCOLUMNSIZER_H */

