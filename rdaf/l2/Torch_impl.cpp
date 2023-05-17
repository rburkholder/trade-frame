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
 * File:    Torch_impl.cpp
 * Author:  raymond@burkholder.net
 * Project: rdaf/l2
 * Created: 2023/05/16 18:00:31
 */

#include "Torch_impl.hpp"

namespace Strategy {

#define FUSION_VECTOR_REFERENCES(z,n,level ) \
  BOOST_PP_COMMA_IF(n) \
  Accumulator( level.BOOST_PP_ARRAY_ELEM(n,ARRAY_NAMES ) )

Torch_impl::Torch_impl( const ou::tf::iqfeed::l2::FeatureSet& fs )
: m_bTimeStepsFilled( false )
, m_ixTimeStep {}
, m_fvAccumulator_l1(
    BOOST_PP_REPEAT( ARRAY_NAMES_SIZE, FUSION_VECTOR_REFERENCES, fs.FVS()[ 1 ] )
  )
, m_fvAccumulator_l2(
    BOOST_PP_REPEAT( ARRAY_NAMES_SIZE, FUSION_VECTOR_REFERENCES, fs.FVS()[ 2 ] )
  )
, m_fvAccumulator_l3(
    BOOST_PP_REPEAT( ARRAY_NAMES_SIZE, FUSION_VECTOR_REFERENCES, fs.FVS()[ 3 ] )
  )
{}

Torch_impl::~Torch_impl() {}

void Torch_impl::Accumulate() {
  boost::fusion::for_each(
    m_fvAccumulator_l1,
    []( auto& accumulator ){
      accumulator.accumulate += accumulator.feature;
      accumulator.count++;
    }
  );
  boost::fusion::for_each(
    m_fvAccumulator_l2,
    []( auto& accumulator ){
      accumulator.accumulate += accumulator.feature;
      accumulator.count++;
    }
  );
  boost::fusion::for_each(
    m_fvAccumulator_l3,
    []( auto& accumulator ){
      accumulator.accumulate += accumulator.feature;
      accumulator.count++;
    }
  );
}

Torch::Op Torch_impl::StepModel( boost::posix_time::ptime dt ) {
  // todo: incorporate dt, or seconds from midnight
  Torch::Op op( Torch::Op::Neutral );

  rTimeStep_Averages_t& step( m_rTimeSteps[ m_ixTimeStep ] );
  rTimeStep_Averages_t::iterator iterTimeStep( step.begin() );

  boost::fusion::for_each(
    m_fvAccumulator_l1,
    [&iterTimeStep]( auto& accumulator ){
      if ( 0 == accumulator.count ) {
        *iterTimeStep = 0.0;
      }
      else {
        *iterTimeStep = accumulator.accumulate / accumulator.count;
        accumulator.Clear();
      }
      iterTimeStep++;
    }
  );

  boost::fusion::for_each(
    m_fvAccumulator_l2,
    [&iterTimeStep]( auto& accumulator ){
      if ( 0 == accumulator.count ) {
        *iterTimeStep = 0.0;
      }
      else {
        *iterTimeStep = accumulator.accumulate / accumulator.count;
        accumulator.Clear();
      }
      iterTimeStep++;
    }
  );

  boost::fusion::for_each(
    m_fvAccumulator_l3,
    [&iterTimeStep]( auto& accumulator ){
      if ( 0 == accumulator.count ) {
        *iterTimeStep = 0.0;
      }
      else {
        *iterTimeStep = accumulator.accumulate / accumulator.count;
        accumulator.Clear();
      }
      iterTimeStep++;
    }
  );

  if ( m_bTimeStepsFilled ) {
    // submit to torch
    //   m_ixTimeStep has latest step
  }

  m_ixTimeStep++;
  if ( c_nTimeSteps == m_ixTimeStep ) {
    m_ixTimeStep = 0;
    m_bTimeStepsFilled = true;
  }

  return op; // placeholder
}

} // namespace Strategy