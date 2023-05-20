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

//#include <torch/torch.h>

#include <ATen/core/ATen_fwd.h>
#include <ATen/ops/from_blob.h>

#include <c10/core/DeviceType.h>
#include <c10/util/Exception.h>

#include "Torch_impl.hpp"

// https://pytorch.org/cppdocs/
// https://g-airborne.com/bringing-your-deep-learning-model-to-production-with-libtorch-part-2-tracing-your-pytorch-model/

namespace Strategy {

#define FUSION_VECTOR_REFERENCES(z,n,level ) \
  BOOST_PP_COMMA_IF(n) \
  Accumulator( level.BOOST_PP_ARRAY_ELEM(n,ARRAY_NAMES ) )

Torch_impl::Torch_impl( const std::string& sTorchModel, const ou::tf::iqfeed::l2::FeatureSet& fs )
: m_ixTimeStep {}
, m_fvAccumulator_l1(
    BOOST_PP_REPEAT( ARRAY_NAMES_SIZE, FUSION_VECTOR_REFERENCES, fs.FVS()[ 1 ] )
  )
, m_fvAccumulator_l2(
    BOOST_PP_REPEAT( ARRAY_NAMES_SIZE, FUSION_VECTOR_REFERENCES, fs.FVS()[ 2 ] )
  )
, m_fvAccumulator_l3(
    BOOST_PP_REPEAT( ARRAY_NAMES_SIZE, FUSION_VECTOR_REFERENCES, fs.FVS()[ 3 ] )
  )
{
  m_vTensor.reserve( c_nTimeSteps );

  try {
    torch::manual_seed(0);
    m_module = torch::jit::load( sTorchModel );
    m_module.to(torch::kCPU);
    //torch::NoGradGuard no_grad_;  // ?

    for ( const auto& attr: m_module.named_attributes() ) {
      std::cout << "attr: " << attr.name << std::endl;
    }

    for (const auto& method: m_module.get_methods() ) {
      std::cout << "method: " << method.name() << std::endl;
    }
  }
  catch ( const c10::Error& e ) {
    std::string s( "torch error: " + e.msg() );
    std::cout << s << std::endl;
    assert( false );
  }
}

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

  auto seconds = dt.time_of_day().seconds();

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

  *iterTimeStep = seconds;

  // https://pytorch.org/cppdocs/api/structc10_1_1_i_value.html
  // IValues contain their values as an IValue::Payload,
  //    which holds primitive types (int64_t, bool, double, Device) and Tensor as values,
  //    and all other types as a c10::intrusive_ptr.
  // https://pytorch.org/cppdocs/notes/tensor_creation.html

  if ( c_nTimeSteps == m_vTensor.size() ) {
    m_vTensor.erase( m_vTensor.begin() );
  }

  auto options = torch::TensorOptions().dtype(torch::kFloat32).device(torch::kCPU, 1);
  m_vTensor.push_back( torch::from_blob( &step, {ARRAY_NAMES_SIZE + 1}, options ) );
  torch::TensorList list = torch::TensorList( m_vTensor );
  torch::Tensor steps = torch::stack( list, 1 );

  std::vector<torch::jit::IValue> inputs;
  inputs.push_back( list );  // choose this one
  inputs.push_back( steps ); // or this one

  assert( false );  // requires tensor construction from above

  if ( c_nTimeSteps == m_vTensor.size() ) {
    // submit to torch
    //   m_ixTimeStep has latest step
    m_module.eval();
    auto output = m_module.forward(inputs).toTuple()->elements()[0].toTensor();
  }

  m_ixTimeStep++;
  assert( m_ixTimeStep <= c_nTimeSteps );
  if ( c_nTimeSteps == m_ixTimeStep ) {
    m_ixTimeStep = 0;
  }

  return op; // placeholder
}

} // namespace Strategy