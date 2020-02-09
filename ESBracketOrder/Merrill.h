/************************************************************************
 * Copyright(c) 2020, One Unified. All rights reserved.                 *
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
 * File:    Merrill.h
 * Author:  raymond@burkholder.net
 * Project: ESBracketOrder
 * Created: February 8, 2020, 15:41 PM
 */

#include <map>
#include <string>

namespace ou {
namespace tf {
namespace Merrill {

enum class EPattern {
  UnDefined = 0,
  DownTrend, InvertedHeadAndShoulders,
  UpTrend,   HeadAndShoulders,
  Broadening, Uninteresting, Triangle
};

using result_t = std::pair<EPattern,const std::string&>;

void Validate();
result_t Classify( double, double, double, double, double );
const std::string& Name( EPattern );

} // namespace Merrill
} // namespace tf
} // namespace ou