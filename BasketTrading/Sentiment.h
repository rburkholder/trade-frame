/*** *********************************************************************
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
 * File:   Sentiment.h
 * Author:    raymond@burkholder.net
 * copyright: 2019 Raymond Burkholder
 *
 * Created on March 26, 2019, 10:05 PM
 */

#ifndef SENTIMENT_H
#define SENTIMENT_H

#include <TFTimeSeries/DatedDatum.h>

class Sentiment {
public:
  Sentiment();
  void Update( const ou::tf::Bar& bar );
  void Get( size_t& nUp_, size_t& nDown_ ) const;
protected:
private:

  size_t nUp;
  size_t nDown;
  ptime dtCurrent; // late arrivals don't count

  void Reset( ptime dtNew );
};

#endif /* SENTIMENT_H */

