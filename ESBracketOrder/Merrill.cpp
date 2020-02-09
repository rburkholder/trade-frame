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
 * File:    Merrill.cpp
 * Author:  raymond@burkholder.net
 * Project: ESBracketOrder
 * Created: February 8, 2020, 15:41 PM
 */

// page 94 in Bollinger Bands

#include <vector>
#include <cassert>

#include "Merrill.h"

namespace {

  std::string sEmpty;

  using EPattern = ou::tf::Merrill::EPattern;

  struct Pattern {
    std::string sName;
    EPattern pattern;
  };

  using mapPattern_t = std::map<std::string,Pattern>;
  mapPattern_t m_mapPattern {
    { "21435", { "M1", EPattern::DownTrend } },
    { "21534", { "M2", EPattern::InvertedHeadAndShoulders } },
    { "31425", { "M3", EPattern::DownTrend } },
    { "31524", { "M4", EPattern::InvertedHeadAndShoulders } },
    { "32415", { "M5", EPattern::Broadening } },
    { "32514", { "M6", EPattern::InvertedHeadAndShoulders } },
    { "41325", { "M7", EPattern::Uninteresting } },
    { "41523", { "M8", EPattern::InvertedHeadAndShoulders } },
    { "42315", { "M9", EPattern::Uninteresting } },
    { "42513", { "M10", EPattern::InvertedHeadAndShoulders } },
    { "43512", { "M11", EPattern::InvertedHeadAndShoulders } },
    { "51324", { "M12", EPattern::Uninteresting } },
    { "51423", { "M13", EPattern::Triangle } },
    { "52314", { "M14", EPattern::Uninteresting } },
    { "52413", { "M15", EPattern::UpTrend } },
    { "53412", { "M16", EPattern::UpTrend } },

    { "13254", { "W1", EPattern::DownTrend } },
    { "14253", { "W2", EPattern::DownTrend } },
    { "14352", { "W3", EPattern::Uninteresting } },
    { "15243", { "W4", EPattern::Triangle } },
    { "15342", { "W5", EPattern::Uninteresting } },
    { "23154", { "W6", EPattern::HeadAndShoulders } },
    { "24153", { "W7", EPattern::HeadAndShoulders } },
    { "24351", { "W8", EPattern::Uninteresting } },
    { "25143", { "W9", EPattern::HeadAndShoulders } },
    { "25341", { "W10", EPattern::Uninteresting } },
    { "34152", { "W11", EPattern::HeadAndShoulders } },
    { "34251", { "W12", EPattern::Broadening } },
    { "35142", { "W13", EPattern::HeadAndShoulders } },
    { "35241", { "W14", EPattern::UpTrend } },
    { "45132", { "W15", EPattern::HeadAndShoulders } },
    { "45231", { "W16", EPattern::UpTrend } }
    }
    ;

  std::vector<std::string> vNames {
    { "UnDefined" },
    { "DownTrend" },
    { "InvertedHeadAndShoulders" },
    { "UpTrend" },
    { "HeadAndShoulders" },
    { "Broadening" },
    { "Uninteresting" },
    { "Triangle" }
  };

}

namespace ou {
namespace tf {
namespace Merrill {

void Validate() {
  for ( auto& pair: m_mapPattern ) {
    auto [ key,value ] = pair;
    assert( 5 == key.size() );
    assert( std::string::npos != key.find( "1" ) );
    assert( std::string::npos != key.find( "2" ) );
    assert( std::string::npos != key.find( "3" ) );
    assert( std::string::npos != key.find( "4" ) );
    assert( std::string::npos != key.find( "5" ) );
  }
}

const std::string& Name( EPattern pattern ) {
  return vNames[ (size_t)pattern ];
}


result_t Classify( double p1, double p2, double p3, double p4, double p5 ) {

  using mapSort_t = std::multimap<double,std::string>;
  mapSort_t mapSort {
    { p1, "1" },
    { p2, "2" },
    { p3, "3" },
    { p4, "4" },
    { p5, "5" },
    };

  std::string sequence;
  for ( auto& pair: mapSort ) {
    auto [key,value] = pair;
    sequence = value + sequence; // ranked top down on chart
  }

  assert( 5 == sequence.size() );

  mapPattern_t::iterator iter = m_mapPattern.find( sequence );
  if ( m_mapPattern.end() == iter ) {
    return result_t( EPattern::UnDefined, sEmpty);
  }
  else return result_t( iter->second.pattern, iter->second.sName );

}

} // namespace Merrill
} // namespace tf
} // namespace ou