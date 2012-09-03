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

// As there are no 64 bit libraries from IQFeed, 
// IQFeed needs to be started manually or from a 32 bit application

#include "IQ32.H"

void __stdcall IQFeedCallBack( int x, int y );

class IQFeed32 {
public:
  IQFeed32( void );
  virtual ~IQFeed32( void );
protected:
private:
};