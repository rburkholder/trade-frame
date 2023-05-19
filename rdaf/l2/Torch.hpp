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
 * File:    Torch.hpp
 * Author:  raymond@burkholder.net
 * Project: rdaf/l2
 * Created: 2023/05/15 21:18:40
 */

#pragma once

#include <boost/date_time/posix_time/ptime.hpp>

#include <memory>

namespace ou {
namespace tf {
namespace iqfeed {
namespace l2 {
  class FeatureSet;
} // namespace l2
} // namespace iqfeed
} // namespace tf
} // namespace ou

namespace Strategy {

class Torch_impl;

class Torch {
public:

  Torch( const std::string& sTorchModel, const ou::tf::iqfeed::l2::FeatureSet& );
  ~Torch();

  enum Op { Long, Neutral, Hold, Short };

  void Accumulate();
  Op StepModel( boost::posix_time::ptime );

protected:
private:

  using pTorch_impl_t = std::unique_ptr<Torch_impl>;
  pTorch_impl_t m_pTorch_impl;

};

} // namespace Strategy