/************************************************************************
 * Copyright(c) 2009, One Unified. All rights reserved.                 *
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

#include "stdafx.h"

#include "TimeSeries.h"

/*
A simple dataspace, H5S_SIMPLE, is a multidimensional array of elements. 
The dimensionality of the dataspace (or the rank of the array) is fixed and 
is defined at creation time. The size of each dimension can grow during 
the life time of the dataspace from the current size up to the maximum 
size. Both the current size and the maximum size are specified at creation 
time. The sizes of dimensions at any particular time in the life of a 
dataspace are called the current dimensions, or the dataspace extent. 
They can be queried along with the maximum sizes.
Simple dataspace
  rank:int
  current_size:hsize_t[rank]  
  maximum_size:hsize_t[rank]

The size of a current dimension cannot be greater than the maximum size, 
which can be unlimited, specified as H5S_UNLIMITED.

if a dataset uses a dataspace with any unlimited dimension, chunking has to be used

HDF5 uses C storage conventions, assuming that the last listed dimension is the 
fastest-changing dimension and the first-listed dimension is the slowest changing.

*/

namespace ou { // One Unified
namespace tf { // TradeFrame

} // namespace tf
} // namespace ou
