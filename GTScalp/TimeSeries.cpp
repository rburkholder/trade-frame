#include "StdAfx.h"
#include "TimeSeries.h"

using namespace H5;

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

// CBars

CBars::CBars(void) {
}

CBars::CBars( unsigned int size )
:CTimeSeries<CBar>( size ) {
}

CBars::~CBars( void ) {
  //CTimeSeries<CBar>::~CTimeSeries();
}

// CTrades

CTrades::CTrades(void) {
}

CTrades::CTrades( unsigned int size )
:CTimeSeries<CTrade>( size ) {
}

CTrades::~CTrades( void ) {
  //CTimeSeries<CTrade>::~CTimeSeries();
}

// CQuotes

CQuotes::CQuotes(void) {
}

CQuotes::CQuotes( unsigned int size )
:CTimeSeries<CQuote>( size ) {
}

CQuotes::~CQuotes( void ) {
  //CTimeSeries<CQuote>::~CTimeSeries();
}

// CMarketDepths

CMarketDepths::CMarketDepths() {
}

CMarketDepths::~CMarketDepths() {
}

CMarketDepths::CMarketDepths(unsigned int size) 
: CTimeSeries<CMarketDepth>( size ) {
}

