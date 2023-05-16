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

#include <vector>

namespace ou {
namespace tf {
namespace iqfeed {
namespace l2 {
  class FeatureSet;
}
}
}
}

namespace Strategy {

class Futures_Torch {
public:

  Futures_Torch();
  ~Futures_Torch();

  enum Op { Long, Neutral, Hold, Short };

  void Accumulate( const ou::tf::iqfeed::l2::FeatureSet& );
  Op StepModel();

protected:
private:
  struct Accumulator {
    double accumulate;
    double count;
  };

  using vAccumulator_t = std::vector<Accumulator>;
  vAccumulator_t mvAccumulator;
  // array 3xn (n to be determined from tuple in impl class)
};

}