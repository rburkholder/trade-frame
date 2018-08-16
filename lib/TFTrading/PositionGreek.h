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
 * File:   PositionGreek.h
 * Author: raymond@burkholder.net
 *
 * Created on July 31, 2018, 9:21 AM
 */

#ifndef POSITIONGREEK_H
#define POSITIONGREEK_H

#include <boost/serialization/version.hpp>
#include <boost/serialization/split_member.hpp>

#include <TFOptions/Option.h>

#include "Position.h"
#include "Watch.h"

namespace ou { // One Unified
namespace tf { // TradeFrame

class PositionGreek: public Position {
  friend class boost::serialization::access;
public:
  
  friend std::ostream& operator<<( std::ostream& os, const PositionGreek& );
  
  typedef boost::shared_ptr<PositionGreek> pPositionGreek_t;
  
  typedef ou::tf::Watch::pWatch_t pUnderlying_t;;
  typedef ou::tf::option::Option::pOption_t pOption_t;
  
  typedef ProviderInterfaceBase::pProvider_t pProvider_t;
  
  PositionGreek( pOption_t&, pUnderlying_t& );
  virtual ~PositionGreek( );
  
  pOption_t GetOption() { return m_pOption; }
  pUnderlying_t GetUnderlying() { return m_pUnderlying; }
  
  ou::Delegate<const ou::tf::Greek&> OnGreek; // need to fire this on option updates
  
  void PositionPendingDelta( int n );  // -1 or +1
  
protected:
  
  typedef SymbolBase::greek_t greek_t;

private:
  
  pOption_t m_pOption;
  pUnderlying_t m_pUnderlying;
  
  int m_nQuantity;  // number of contracts
  
  void Construction();

  void HandleGreek( greek_t );
  
  template<typename Archive>
  void save( Archive& ar, const unsigned int version ) const {
    ar & boost::serialization::base_object<const Position>(*this);
  }

  template<typename Archive>
  void load( Archive& ar, const unsigned int version ) {
    ar & boost::serialization::base_object<Position>(*this);
    OnPositionChanged( *this );  // may need this in position.h as well.
  }

  BOOST_SERIALIZATION_SPLIT_MEMBER()

};

std::ostream& operator<<( std::ostream& os, const PositionGreek& );

} // namespace tf
} // namespace ou

BOOST_CLASS_VERSION(ou::tf::PositionGreek, 1)
  
#endif /* POSITIONGREEK_H */

