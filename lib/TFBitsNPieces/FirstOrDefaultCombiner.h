/************************************************************************
 * Copyright(c) 2017, One Unified. All rights reserved.                 *
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
 * File:   FirstOrDefaultCombiner.h
 * Author: rpb
 *
 * Created on February 25, 2017, 10:21 PM
 */

#pragma once

namespace ou {
namespace tf { 
  
template<typename T>
struct FirstOrDefault {  // a combiner used for signals with return values
  typedef T result_type;
  template<typename InputIterator>
  T operator()( InputIterator first, InputIterator last ) const {
    if (first==last) return T();
    return *first;
  }
};

} // namespace tf
} // namespace ou
