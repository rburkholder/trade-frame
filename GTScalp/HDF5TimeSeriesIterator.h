#pragma once

#include "DatedDatum.h"

#include "DataManager.h"
using namespace H5;

#include "HDF5TimeSeriesAccessor.h"

#include <iterator>
//using namespace std;

template<class T> class CHDF5TimeSeriesIterator: 
  public std::iterator<std::random_access_iterator_tag, const CDatedDatum, hsize_t> {
public:
  explicit CHDF5TimeSeriesIterator<T>( CHDF5TimeSeriesAccessor<T> *pAccessor ); // end() init
  explicit CHDF5TimeSeriesIterator<T>( CHDF5TimeSeriesAccessor<T> *pAccessor, hsize_t Index );  // begin() or later init
  ~CHDF5TimeSeriesIterator<T>(void) { };
  CHDF5TimeSeriesIterator<T>( const CHDF5TimeSeriesIterator<T>& other );  // copy constructor
  //CHDF5TimeSeriesIterator &
protected:
  CHDF5TimeSeriesAccessor<T> *m_pAccessor;
  bool m_bPointingSomewhere;  // m_ItemIndex is valid ie, is .end()
  hsize_t m_ItemIndex; // index into specific item
  T m_T; // the currently retrieved datum
private:
};

template<class T> CHDF5TimeSeriesIterator<T>::CHDF5TimeSeriesIterator( CHDF5TimeSeriesAccessor<T> *pAccessor ):
  m_pAccessor( pAccessor ), 
  m_ItemIndex( 0 ),
  m_bPointingSomewhere( false )
   {
}

template<class T> CHDF5TimeSeriesIterator<T>::CHDF5TimeSeriesIterator( CHDF5TimeSeriesAccessor<T> *pAccessor, hsize_t Index ):
  m_pAccessor( pAccessor ), 
  m_ItemIndex( Index ),
  m_bPointingSomewhere( false ) {   // need to be get the item retrieved before setting this to true
    // need to do the retrieval and set the flag
}

template<class T> CHDF5TimeSeriesIterator<T>::CHDF5TimeSeriesIterator( const CHDF5TimeSeriesIterator<T>& other ) :
  m_pAccessor( other.m_pAccessor ),
  m_ItemIndex( other.m_ItemIndex ),
  m_bPointingSomewhere( true )
  //m_pDatum( newother.m_pDatum )  // need to copy the datum
  {

}