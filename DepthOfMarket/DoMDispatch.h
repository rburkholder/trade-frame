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
 * File:    DoMDispatch.h
 * Author:  raymond@burkholder.net
 * Project: DepthOfMarket
 * Created on October 17, 2021 11:45
 */

#pragma once

#include <TFIQFeed/Level2/Dispatcher.h>

// NOTE: implement one per symbol
//   might be somewhat faster with out a symbol lookup on each message

class DoMDispatch
  : public ou::tf::iqfeed::l2::Dispatcher<DoMDispatch>
{
  friend ou::tf::iqfeed::l2::Dispatcher<DoMDispatch>;
public:
protected:
private:
};