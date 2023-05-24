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

// https://pytorch.org/cppdocs/
// https://g-airborne.com/bringing-your-deep-learning-model-to-production-with-libtorch-part-1-why-libtorch/
// https://g-airborne.com/bringing-your-deep-learning-model-to-production-with-libtorch-part-2-tracing-your-pytorch-model/
// https://g-airborne.com/bringing-your-deep-learning-model-to-production-with-libtorch-part-3-advanced-libtorch/

// https://medium.com/crim/from-pytorch-to-libtorch-tips-and-tricks-dc45b6c1b1ac

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

    torch::manual_seed( 0 );

    m_tensorCell = torch::zeros( { 1, 1, 64 } );
    m_tensorHidden = torch::zeros( { 1, 1, 64 } );

    m_module = torch::jit::load( sTorchModel );
    m_module.to( torch::kCPU );
    m_module.train( false );

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

Torch::Op Torch_impl::StepModel( boost::posix_time::ptime dt, Torch::Op op_old_t, double unrealized, float result[3] ) {

  auto seconds = dt.time_of_day().total_seconds();

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

  m_vTensor.push_back( torch::from_blob( &step, { 1, c_nLevels * ARRAY_NAMES_SIZE + 1 }, options ).requires_grad_( false ) );
  torch::Tensor steps = torch::stack( m_vTensor, 1 );

  assert( c_nTimeSteps >= m_vTensor.size() );

  inputs.push_back( steps ); // or this one

  double dblOpOld {};
  switch ( op_old_t ) {
    case Torch::Op::Hold:
      break;
    case Torch::Op::Long:
      dblOpOld = +1.0;
      break;
    case Torch::Op::Neutral:
      dblOpOld =  0.0;
      break;
    case Torch::Op::Short:
      dblOpOld = -1.0;
      break;
  }

  float state[ 1 ][ 2 ];
  state[ 0 ][ 0 ] = dblOpOld;
  state[ 0 ][ 1 ] = unrealized;
  torch::Tensor state_tensor = torch::from_blob( &state, { 1, 2 } ).requires_grad_( false );
  inputs.push_back( state_tensor );

  std::vector<torch::jit::IValue> tuple;
  tuple.push_back( m_tensorHidden );
  tuple.push_back( m_tensorCell );
  inputs.push_back(torch::ivalue::Tuple::create( tuple ) );

  Torch::Op op { Torch::Op::Neutral };

  if ( c_nTimeSteps == m_vTensor.size() ) {
    // submit to torch
    // m_module.eval();

    torch::NoGradGuard no_grad_;

    auto output = m_module.forward( inputs );

    auto recycle = output.toTuple()->elements()[ 1 ];
    m_tensorHidden = recycle.toTuple()->elements()[0].toTensor();
    m_tensorCell = recycle.toTuple()->elements()[1].toTensor();

    torch::Tensor trade = output.toTuple()->elements()[ 0 ].toTensor();

    result[ 0 ] = trade[ 0 ][ 0 ].item<float>(); // short
    result[ 1 ] = trade[ 0 ][ 1 ].item<float>(); // neutral
    result[ 2 ] = trade[ 0 ][ 2 ].item<float>(); // long

    float& short_( result[ 0 ] );
    float& neutral_( result[ 1 ] );
    float& long_( result[ 2 ] );

    // not normalized
    //assert( 0 <= short_ );
    //assert( 0 <= neutral_ );
    //assert( 0 <= long_ );

    //float sum( short_ );
    //sum += neutral_;
    //sum += long_;

    //assert( 0.98 < sum );

    if ( neutral_ < short_ ) {
      if ( short_ < long_ ) {
        op = Torch::Op::Long;
      }
      else {
        op = Torch::Op::Short;
      }
    }
    else {
      if ( neutral_ < long_ ) {
        if ( long_ < short_ ) {
          op = Torch::Op::Short;
        }
        else {
          op = Torch::Op::Long;
        }
      }
    }

  }

  m_ixTimeStep++;
  assert( m_ixTimeStep <= c_nTimeSteps );
  if ( c_nTimeSteps == m_ixTimeStep ) {
    m_ixTimeStep = 0;
  }

  return op; // placeholder
}

} // namespace Strategy
