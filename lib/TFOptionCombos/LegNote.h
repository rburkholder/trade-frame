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
  enum class Type {
    SynthLong = 0, SynthShort = 1, Cover = 2, Protect = 3,
    DltaPlsGmPls = 5, DltaPlsGmMns = 6, DltaMnsGmPls = 7, DltaMnsGmMns = 8,
    Long = 11, Short = 12
    };
  enum class Side { Long, Short };
  enum class Option { Call, Put };
  enum class Momentum { Rise, Fall, Unknown };
  enum class Algo { Collar };

  struct values_t {
    State m_state;
    Type m_type;
    Side m_side;
    Option m_option;
    Momentum m_momentum;
    Algo m_algo;
    bool m_lock;
    double m_iv;
    values_t(): m_lock( false ), m_iv {} {}
  };

  LegNote();
  LegNote( const LegNote&& );
  LegNote( const values_t& );
  LegNote( const std::string& ); // construct from formatted string, typically saved in Position
  LegNote& operator=( const LegNote&& );
  LegNote( const LegNote& ) = delete;
  virtual ~LegNote();

  static std::string LU( Type );

  const values_t& Decode( const std::string& );
  const std::string Encode() const;

  const values_t& Values() const;

  void Assign( const values_t& );

  State GetState() const { return m_values.m_state; }
  void  SetState( State state ) { m_values.m_state = state; }

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
