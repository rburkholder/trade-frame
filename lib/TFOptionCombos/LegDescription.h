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
 * File:    LegDescription.h
 * Author:  raymond@burkholder.net
 * Project: TFOptionCombos
 * Created: January 1, 2021, 15:50
 *
 * provides text for Position record
 */

#ifndef LEGDESCRIPTION_H
#define LEGDESCRIPTION_H

#include <string>

class LegDescription {
public:

  // leg: [1,2,...]
  enum class Type { SynthLong, SynthShort, Cover, Protect };
  enum class State { Open, Expired, Closed };
  enum class Option { Call, Put };
  enum class Side { Long, Short };
  enum class Momentum { Rise, Fall };
  enum class Algo { Collar };

  struct values_t {
    unsigned int m_ixLeg;
    Type m_type;
    State m_state;
    Option m_option;
    Side m_side;
    Momentum m_momentum;
    Algo m_algo;
  };

  LegDescription() {}
  LegDescription( const std::string& ); // construct from formatted string
  virtual ~LegDescription();

  const values_t& Decode( const std::string& );
  const std::string Encode() const;

  const values_t& Values() const;

  void Assign( const values_t& );

protected:
private:

  values_t m_values;

  void Parse( const std::string& );

};

#endif /* LEGDESCRIPTION_H */
