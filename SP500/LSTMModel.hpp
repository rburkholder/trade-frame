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
 * File:    LSTMModel.hpp
 * Author:  raymond@burkholder.net
 * Project: SP500
 * Created: July 4, 2025 11:32:42
 */

#pragma once

#include <memory>
#include <vector>
#include <cstring>

#include <c10/util/ArrayRef.h>

#include "c10/core/Device.h"

class LSTM;
class Features_raw;
class Features_scaled;
class HyperParameters;

class Model {
public:

  Model( const std::string& sDevice, std::int8_t ixDevice, double dblLossTarget );
  ~Model();

  void Append( const Features_raw&, Features_scaled& ); // training mode and prediction mode

  void Train_Init();
  void Train_BuildSamples();
  void Train_Perform( const HyperParameters& ); // train on batch of Append'd values

  void EnablePredictionMode();
  float Predict();

  size_t PredictionDistance() const;

  void Save( const std::string& );
  void Load( const std::string& );

  using rPrediction_t = c10::ArrayRef<float>;
  using fPredictionResult_t = std::function<void( const rPrediction_t& )>;
  void SetPredictionResult( fPredictionResult_t&& );

protected:
private:

  double m_dblLossTarget;

  c10::Device m_torchDevice;

  // will be attempting to predict normalized value referenced at ixTrade
  enum EFeature {
    ixTrade = 0
  , ixSDDirection
  , ixEma029, ixEma013
  , ixTickj, ixTickl
  , ixRtnMean, ixRtnSlp
  , ixQuoteImbalance
  , nInputFeature_ };

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
    fields_t( const type trade
            , const type SDDirection
            , const type ema029, const type ema013
            , const type tickj, const type tickl
            , const type rtn_mean, const type rtn_slope
            , const type quote_imbalance
    ) {
      fields[ ixEma029 ] = ema029;
      fields[ ixEma013 ] = ema013;
      fields[ ixTrade  ] = trade;
      fields[ ixSDDirection ] = SDDirection;
      fields[ ixTickj  ] = tickj;
      fields[ ixTickl  ] = tickl;
      fields[ ixRtnMean ] = rtn_mean;
      fields[ ixRtnSlp ] = rtn_slope;
      fields[ ixQuoteImbalance ] = quote_imbalance;
    }
  };

  using vValuesFlt_t = std::vector<fields_t<float> >;
  vValuesFlt_t m_vDataScaled; // LSTM prefers float, values are 0.0 to 1.0 anyway
  vValuesFlt_t::size_type m_ixDataScaled;

  long m_nSamples_actual {};

  vValuesFlt_t m_vSourceForTensorX; // implicit 3 dimensions:  [sample index][sample size in seconds][feature list]
  std::vector<float> m_vSourceForTensorY; // [samples match X][1 second for prediction][last index implies 1 feature]

  using pLSTM_t = std::unique_ptr<LSTM>;
  pLSTM_t m_pLSTM;

  fPredictionResult_t m_fPredictionResult;

  static bool Scale( const Features_raw&, Features_scaled& );

};