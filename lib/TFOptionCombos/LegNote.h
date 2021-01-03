/************************************************************************
 * Copyright(c) 2021, One Unified. All rights reserved.                 *
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
 * File:    LegNote.h
 * Author:  raymond@burkholder.net
 * Project: TFOptionCombos
 * Created: January 1, 2021, 15:50
 *
 * provides text for Position notes
 */

#ifndef LEGNOTE_H
#define LEGNOTE_H

#include <string>

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace option { // options

class LegNote {
public:

  enum class State { Open, Expired, Closed };
  enum class Type { SynthLong, SynthShort, Cover, Protect };
  enum class Side { Long, Short };
  enum class Option { Call, Put };
  enum class Momentum { Rise, Fall };
  enum class Algo { Collar };

  struct values_t {
    State m_state;
    Type m_type;
    Side m_side;
    Option m_option;
    Momentum m_momentum;
    Algo m_algo;
  };

  LegNote();
  LegNote( const values_t& );
  LegNote( const std::string& ); // construct from formatted string
  LegNote( const LegNote&& );
  LegNote( const LegNote& );
  LegNote& operator=( const LegNote&& );
  virtual ~LegNote();

  const values_t& Decode( const std::string& );
  const std::string Encode() const;

  const values_t& Values() const;

  void Assign( const values_t& );

protected:
private:

  bool m_bValid;

  values_t m_values;

  void Parse( const std::string& );

};

} // namespace option
} // namespace tf
} // namespace ou

#endif /* LEGNOTE_H */
