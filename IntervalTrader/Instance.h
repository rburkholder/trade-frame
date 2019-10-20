/************************************************************************
 * Copyright(c) 2019, One Unified. All rights reserved.                 *
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
 * File:    Instance.h
 * Author:  raymond@burkholder.net
 * Project: IntervalSampler
 * Created on October 6, 2019, 9:12 PM
 */

#ifndef INSTANCE_H
#define INSTANCE_H

#include <functional>

//#include <TFTrading/Watch.h>
#include <TFTrading/Position.h>

class Instance {
public:

  using pWatch_t = ou::tf::Watch::pWatch_t;
  using pPosition_t = ou::tf::Position::pPosition_t;

  using fEvaluate_t = std::function<void( double, pPosition_t )>; // accumulated dollar volume, position

  Instance( pPosition_t );
  virtual ~Instance();

  void Evaluate( fEvaluate_t&& );
protected:
private:

  pWatch_t m_pWatch;
  pPosition_t m_pPosition;

  bool m_bIntervalHasTrades;

  double m_spread;

  double m_dblOpen;
  double m_dblClose;

  ou::tf::Trade::volume_t m_volume;

  void HandleQuote( const ou::tf::Quote& quote );
  void HandleTrade( const ou::tf::Trade& trade );

};

#endif /* INSTANCE_H */

