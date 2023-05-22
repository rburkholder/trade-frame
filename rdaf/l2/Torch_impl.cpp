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

#include <tuple>

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

    m_tensorCell = torch::zeros( { 1, 1, 64 } );
    m_tensorHidden = torch::zeros( { 1, 1, 64 } );

    m_module = torch::jit::load( sTorchModel );
    m_module.to(torch::kCPU);
    //torch::NoGradGuard no_grad_;  // ?

    //for ( const auto& attr: m_module.named_attributes() ) {
    //  std::cout << "attr: " << attr.name << std::endl;
    //}

    //for (const auto& method: m_module.get_methods() ) {
    //  std::cout << "method: " << method.name() << std::endl;
    //}
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

Torch::Op Torch_impl::StepModel( boost::posix_time::ptime dt, Torch::Op old_op, double unrealized, float result[3] ) {

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

  std::vector<torch::jit::IValue> inputs;

  static const auto options = torch::TensorOptions().dtype( torch::kFloat32 ).device( torch::kCPU, -1 );

  m_vTensor.push_back( torch::from_blob( &step, { c_nLevels * ARRAY_NAMES_SIZE + 1 }, options ) );
  torch::TensorList listSteps = torch::TensorList( m_vTensor );
  torch::Tensor steps = torch::stack( listSteps, 1 );

  //inputs.push_back( listSteps );  // choose this one
  inputs.push_back( steps ); // or this one

  double currentOp {};
  switch ( old_op ) {
    case Torch::Op::Hold:
      break;
    case Torch::Op::Long:
      currentOp = +1.0;
      break;
    case Torch::Op::Neutral:
      currentOp =  0.0;
      break;
    case Torch::Op::Short:
      currentOp = -1.0;
      break;
  }

  float state[ 2 ];
  state[ 0 ] = currentOp;
  state[ 1 ] = unrealized;
  //torch::Tensor state_tensor = torch::from_blob( &state, { 2 } );
  //inputs.push_back( state_tensor );
  inputs.push_back( torch::from_blob( &state, { 2 } ) );

  std::tuple<torch::Tensor, torch::Tensor> tuple( m_tensorHidden, m_tensorCell );
  //std::vector<torch::jit::IValue> tuple;
  //tuple.push_back( m_tensorHidden );
  //tuple.push_back( m_tensorCell );
  inputs.push_back( tuple );

  Torch::Op op { Torch::Op::Neutral };

  if ( c_nTimeSteps == m_vTensor.size() ) {
    // submit to torch
    //m_module.eval();
    //auto output = m_module.forward(inputs).toTuple()->elements()[0].toTensor();
    auto output = m_module.forward( inputs );

    auto recycle = output.toTuple()->elements()[ 1 ];
    m_tensorHidden = recycle.toTuple()->elements()[0].toTensor();
    m_tensorCell = recycle.toTuple()->elements()[1].toTensor();

    torch::Tensor trade = output.toTuple()->elements()[ 0 ].toTensor();
    auto trade_a = trade.accessor<float,1>();
    assert( 3 == trade_a.size( 0 ) );

    result[ 0 ] = trade_a[ 0 ];
    result[ 1 ] = trade_a[ 1 ];
    result[ 2 ] = trade_a[ 2 ];

  }

  m_ixTimeStep++;
  assert( m_ixTimeStep <= c_nTimeSteps );
  if ( c_nTimeSteps == m_ixTimeStep ) {
    m_ixTimeStep = 0;
  }

  return op; // placeholder
}

} // namespace Strategy