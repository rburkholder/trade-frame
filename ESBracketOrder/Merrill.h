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

class Merrill {
public:

  enum class EPattern {
    UnDefined,
    DownTrend, InvertedHeadAndShoulders,
    UpTrend,   HeadAndShoulders,
    Broadening, Uninteresting, Triangle
  };

  Merrill();
  virtual ~Merrill();

  EPattern Classify( double, double, double, double, double );

protected:
private:

  struct Pattern {
    std::string sName;
    EPattern pattern;
  };

  using mapPattern_t = std::map<std::string,Pattern>;
  mapPattern_t m_mapPattern;
};

} // namespace ou