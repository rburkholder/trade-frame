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

Torch_impl::Torch_impl()
: m_bTimeStepAccumulation_filled( false )
, m_ixTimeStepAccumulation {}
{}

Torch_impl::~Torch_impl() {}

void Torch_impl::Accumulate( const ou::tf::iqfeed::l2::FeatureSet& fs ) {

  rLevelAccumulation_t::size_type ix {};
  for ( const ou::tf::iqfeed::l2::FeatureSet::vLevels_t::value_type& vt: fs.FVS() ) {
    switch ( ix ) {
      case 0:
        // skip
        break;
      case 1:
      case 2:
      case 3:

        break;
      default:
        break;
    }
    ix++;
  }
}

Torch::Op Torch_impl::StepModel() {
  Torch::Op op( Torch::Op::Neutral );

  // calc average from accumulate/count
  // 3 levels
  // maintain 10 minutes x 60 seconds

  return op; // placeholder
}


} // namespace Strategy