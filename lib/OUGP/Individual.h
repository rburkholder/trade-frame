/************************************************************************
 * Copyright(c) 2012, One Unified. All rights reserved.                 *
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

#include <boost/shared_ptr.hpp>

#include "RootNode.h"

namespace ou { // One Unified
namespace gp { // genetic programming

class Individual {
public:

  unsigned int m_id;

  typedef RootNode* pRootNode_t;

  struct Signals_t {
    pRootNode_t rnLong; // mutually exclusive, when long is set, be long, when short is set, be short, 
    pRootNode_t rnShort;  // when both are set or unset, be out of market
    static const unsigned int  cntSignals = 2;
    template<typename F> // function
    void EachSignal( F f ) {
      f( &rnLong );  // send the address of the pointer
      f( &rnShort ); // send the address of the pointer
    }
    void Replicate( const Signals_t& rhs ) {
      if ( 0 != rhs.rnLong ) rnLong = dynamic_cast<pRootNode_t>( rhs.rnLong->Replicate() );
      if ( 0 != rhs.rnShort ) rnShort = dynamic_cast<pRootNode_t>( rhs.rnShort->Replicate() );
    }
    Signals_t( void ) : rnLong( 0 ), rnShort( 0 ) {}
    Signals_t( const Signals_t& rhs ) : rnLong( 0 ), rnShort( 0 ) { Replicate( rhs ); };
    const Signals_t& operator=( const Signals_t& rhs ) { 
      if ( &rhs != this ) {
        Replicate( rhs ); 
      }
      return *this;
    };
    ~Signals_t( void ) { 
      if ( 0 != rnLong ) { delete rnLong; rnLong = 0; };
      if ( 0 != rnShort ) { delete rnShort; rnShort = 0; };
    }
  } m_Signals;

  double m_dblRawFitness; // absolute dollars value
  double m_dblRelativeFitness;  // maxrawfitness - rawfitness, larger numbers therefore worse (aka standardized fitness)
  double m_dblAdjustedFitness;  // 1 / ( 1 + rf ), range 0 to 1, with 1 being best
  double m_dblNormalizedFitness;  // af / sum(af), range 0 to 1, with 1 being best, sum is 1
  std::stringstream m_ssFormula;

  Individual( void );
  Individual( const Individual& rhs );
  ~Individual( void );

  const Individual& operator=( const Individual& rhs );

  void TreeToString( std::stringstream& ss ) const;

  bool IsComputed( void ) const { return m_bComputed; };
  void SetComputed( bool bComputed = true ) { m_bComputed = bComputed; };

  bool operator>( const Individual& rhs ) const { return m_dblNormalizedFitness > rhs.m_dblNormalizedFitness; };
  bool operator<( const Individual& rhs ) const { return m_dblNormalizedFitness < rhs.m_dblNormalizedFitness; };

protected:
private:
  bool m_bComputed;  // don't waste time recalculating in subsequent populations, reset on CrossOver
  static unsigned int m_nIdGenerator;
  unsigned int m_nCount;  // how many times has this Individual crossed generations
};

} // namespace gp
} // namespace ou
