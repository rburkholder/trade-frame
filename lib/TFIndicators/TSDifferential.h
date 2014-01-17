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

// gamma is 0.0: differential, used for generating return value of the time duration
// gamma is 0.5: stochistic diffusion process
// gamma is 1.0: the usual derivative

class TSDifferential: public Prices { // page 65 Intro to HF Finance
public:
  TSDifferential( Prices& series, time_duration dt );
  TSDifferential( Prices& series, time_duration dt, double dblGammaDerivative, time_duration dtNormalization = hours( 365 * 24 ) );
  TSDifferential( const TSDifferential& rhs );
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
  Prices& m_seriesSource;

  TSEMA<Price>* m_pema1;
  TSEMA<Price>* m_pema2;
  TSEMA<Price>* m_pema3;
  TSEMA<Price>* m_pema4;
  TSEMA<Price>* m_pema5;
  TSEMA<Price>* m_pema6;

  void HandleTerm1Update( const Price& );
  void HandleTerm2Update( const Price& );
  void HandleTerm3Update( const Price& );
  void Init( void );
};

} // namespace hf
} // namespace tf
} // namespace ou
