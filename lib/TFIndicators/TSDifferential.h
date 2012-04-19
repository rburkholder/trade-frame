/************************************************************************
 * Copyright(c) 2012, One Unified. All rights reserved.                 *
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

#pragma once

#include "TSEMA.h"

namespace ou { // One Unified
namespace tf { // TradeFrame
namespace hf { // high frequency

class TSDifferential: public CPrices { // page 65 Intro to HF Finance
public:
  TSDifferential( CPrices& series, time_duration dt );
  TSDifferential( CPrices& series, time_duration dt, double dblGammaDerivative, time_duration dtNormalization = hours( 365 * 24 ) );
  ~TSDifferential(void);
protected:
private:

  static const double m_gamma;
  static const double m_beta;
  static const double m_alpha;
  time_duration m_dtTimeRange;
  time_duration m_dtAlphaTau;
  time_duration m_dtAlphaBetaTau;
  double m_dblTerm1;
  double m_dblTerm2;

  bool m_bDerivative;
  time_duration m_dtNormalization;
  double m_dblNormalization;
  double m_dblGammaDerivative;
  CPrices& m_seriesSource;

  TSEMA<CPrice>* m_pema1;
  TSEMA<CPrice>* m_pema2;
  TSEMA<CPrice>* m_pema3;
  TSEMA<CPrice>* m_pema4;
  TSEMA<CPrice>* m_pema5;
  TSEMA<CPrice>* m_pema6;

  void HandleTerm1Update( const CPrice& );
  void HandleTerm2Update( const CPrice& );
  void HandleTerm3Update( const CPrice& );
  void Init( void );
};

} // namespace hf
} // namespace tf
} // namespace ou
