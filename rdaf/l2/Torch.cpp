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
 * File:    Torch.cpp
 * Author:  raymond@burkholder.net
 * Project: rdaf/l2
 * Created: 2023/05/15 21:18:40
 */

#include "Torch.hpp"
#include "Torch_impl.hpp"

namespace Strategy {

Torch::Torch( const std::string& sTorchModel, const ou::tf::iqfeed::l2::FeatureSet& fs ) {
  m_pTorch_impl = std::make_unique<Torch_impl>( sTorchModel, fs );
}

Torch::~Torch() {
  m_pTorch_impl.reset();
}

void Torch::Accumulate() {
  m_pTorch_impl->Accumulate();
}

Torch::Op Torch::StepModel( boost::posix_time::ptime dt ) {
  return m_pTorch_impl->StepModel( dt );
}

}