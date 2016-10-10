/************************************************************************
 * Copyright(c) 2016, One Unified. All rights reserved.                 *
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
 * File:   InstrumentInfo.h
 * Author: rpb
 *
 * Created on October 8, 2016, 6:58 PM
 */

#ifndef INSTRUMENTINFO_H
#define INSTRUMENTINFO_H

#include <boost/shared_ptr.hpp>

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

#include <TFTrading/Instrument.h>
#include <TFTrading/ProviderManager.h>
#include <TFTrading/Watch.h>

class InstrumentInfo {
public:
  
  typedef ou::tf::Instrument::pInstrument_t pInstrument_t;
  typedef ou::tf::ProviderManager::pProvider_t pProvider_t;
  
  typedef boost::shared_ptr<InstrumentInfo> pInstrumentInfo_t;
    
  InstrumentInfo( pInstrument_t, pProvider_t );
  //InstrumentInfo(const InstrumentInfo& orig);
  virtual ~InstrumentInfo();
  
  bool Watching( void ) const { if ( 0 == m_pWatch ) return false; else return m_pWatch->Watching(); }
  void Watch( void );  // will want to set signals on provider so watch/unwatch as provider transitions connection states
  void UnWatch( void );
  void Emit( void );
  
  pInstrument_t GetInstrument( void ) { return m_pInstrument; }
  
private:

  pInstrument_t m_pInstrument;
  pProvider_t m_pDataProvider;
  
  ou::tf::Watch* m_pWatch;
  
  template<typename Archive>
  void save( Archive& ar, const unsigned int version ) const {
    //ar & boost::serialization::base_object<const InstrumentInfo>(*this);
  }

  template<typename Archive>
  void load( Archive& ar, const unsigned int version ) {
    //ar & boost::serialization::base_object<InstrumentInfo>(*this);
  }

  BOOST_SERIALIZATION_SPLIT_MEMBER()
};

#endif /* INSTRUMENTINFO_H */

