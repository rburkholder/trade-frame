/************************************************************************
 * Copyright(c) 2025, One Unified. All rights reserved.                 *
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
 * File:    Model.hpp
 * Author:  raymond@burkholder.net
 * Project: SP500
 * Created: July 4, 2025 11:32:42
 */

#pragma once

#include <memory>
#include <vector>
#include <cstring>

#include <c10/core/DeviceType.h>

#include <TFTimeSeries/DatedDatum.h>

class LSTM;
class Features_raw;
class Features_scaled;
class HyperParameters;

class Model {
public:

  Model();
  ~Model();

  void Append( const Features_raw&, Features_scaled& );
  void Build( const HyperParameters& );

  ou::tf::Price EmptyPrice( boost::posix_time::ptime );

  void Eval();
  ou::tf::Price Predict( boost::posix_time::ptime );

protected:
private:

  torch::DeviceType m_torchDevice;

  //enum EFeature { ixEma200 = 0, ixEma050, ixEma029, ixEma013, ixTrade, ixTickj, ixTickl, ixAdvdec, nInputFeature_ };
  enum EFeature { ixEma200 = 0, ixEma050, ixEma029, ixEma013, ixTrade, ixTickj, ixTickl, nInputFeature_ };

  template<typename type>
  using rFields_t = type[ nInputFeature_ ];

  template<typename type>
  struct fields_t {
    rFields_t<type> fields;
    fields_t( const fields_t& rhs )
    {
      std::memcpy( fields, rhs.fields, nInputFeature_ * sizeof( type ) );
    }
    // todo: how to bulid an initializer?
    fields_t( const type v1, const type v2, const type v3, const type v4
            , const type v5, const type v6, const type v7 /*, const type v8 */ )
    {
      fields[ ixEma200 ] = v1;
      fields[ ixEma050 ] = v2;
      fields[ ixEma029 ] = v3;
      fields[ ixEma013 ] = v4;
      fields[ ixTrade  ] = v5;
      fields[ ixTickj  ] = v6;
      fields[ ixTickl  ] = v7;
      //fields[ ixAdvdec ] = v8;
    }
  };

  using vValuesFlt_t = std::vector<fields_t<float> >;
  vValuesFlt_t m_vDataScaled; // LSTM prefers float, values are 0.0 to 1.0 anyway
  vValuesFlt_t::const_iterator m_iterDataScaled;

  using pLSTM_t = std::unique_ptr<LSTM>;
  pLSTM_t m_pLSTM;
};